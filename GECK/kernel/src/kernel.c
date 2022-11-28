#include "../include/kernel.h"

t_log *logger;
t_log *logger_debug;

t_configuracion_kernel *config;

int cpu_dispatch_fd;
int cpu_interrupt_fd;
int memoria_fd;

bool esta_usando_rr;
bool volvio_pcb = false; // Tener en cuenta para IO/EXIT/PAGE_FAULT, etc.

t_list *procesosNew;
t_list *procesosReady;
t_list *procesosBajaPrioridad;
t_list *procesosBlock;
t_list *procesosExit;

pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_block;
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_baja_prioridad;

sem_t planificar;
sem_t cpu_idle;
sem_t sem_estructuras_memoria;
sem_t sem_disco;
sem_t sem_impresora;
sem_t sem_procesos_ready;
sem_t sem_proceso_nuevo;

pthread_t hilo_quantum;

char *config_path;

void sighandler(int x) {
	switch (x) {
	case SIGINT:
		terminar_kernel();
		exit(EXIT_SUCCESS);
		break;
	}
}

int main(int argc, char **argv) {
	signal(SIGINT, sighandler);

	if (argc < 2) {
		config_path = "kernel.config";
	} else {
		config_path = argv[1];
	}

	inicializar_kernel();
	log_debug(logger_debug, "config: %s", config_path);

	int server_fd = iniciar_servidor_kernel(config->ip_kernel,
			config->puerto_kernel);

	hilo_memoria();
	hilo_cpu_interrupt();
	hilo_escucha_dispatch();

	send_debug(cpu_interrupt_fd);
	send_debug(cpu_dispatch_fd);
	send_debug(memoria_fd);

	hilo_planificador_largo_plazo();
	hilo_planificador_corto_plazo();

	while (server_kernel(SERVERNAME, server_fd));

	terminar_kernel();

	return EXIT_SUCCESS;
}

void hilo_planificador_largo_plazo() {
	pthread_t hilo;

	pthread_create(&hilo, NULL, (void*) planificador_largo_plazo, NULL);
	pthread_detach(hilo);
}

void hilo_planificador_corto_plazo() {
	pthread_t hilo;

	pthread_create(&hilo, NULL, (void*) planificador_corto_plazo, NULL);
	pthread_detach(hilo);
}

void hilo_memoria() {
	pthread_t hilo;

	memoria_fd = conectar_con("Memoria", config->ip_memoria,
			config->puerto_memoria);

	t_manejar_conexion_args *args = malloc(sizeof(t_manejar_conexion_args));
	args->fd = memoria_fd;
	args->server_name = "Memoria";

	pthread_create(&hilo, NULL, (void*) manejar_comunicacion, (void*) args);
	pthread_detach(hilo);
}

void hilo_cpu_interrupt() {
	pthread_t hilo;

	cpu_interrupt_fd = conectar_con("CPU (interrupt)", config->ip_cpu,
			config->puerto_cpu_interrupt);

	t_manejar_conexion_args *args = malloc(sizeof(t_manejar_conexion_args));
	args->fd = cpu_interrupt_fd;
	args->server_name = "CPU - Interrupt";

	pthread_create(&hilo, NULL, (void*) manejar_comunicacion, (void*) args);
	pthread_detach(hilo);
}

void hilo_escucha_dispatch() {
	pthread_t hilo;

	cpu_dispatch_fd = conectar_con("CPU (dispatch)", config->ip_cpu,
			config->puerto_cpu_dispatch);

	t_manejar_conexion_args *args = malloc(sizeof(t_manejar_conexion_args));
	args->fd = cpu_dispatch_fd;
	args->server_name = "CPU - Dispatch";

	pthread_create(&hilo, NULL, (void*) manejar_comunicacion, (void*) args);
	pthread_detach(hilo);
}

void crear_hilo_quantum() {
	log_debug(logger_debug, "Estoy creando el hilo de quantum :D");
	pthread_create(&hilo_quantum, NULL, (void*) fin_de_quantum, (void*) NULL);
	pthread_detach(hilo_quantum);
}

void matar_hilo_quantum() {
	int success = pthread_cancel(hilo_quantum);
	if (success != 0) {
		log_error(logger_debug,
				"Error intentando matar al hilo de quantum :`(");
	} else {
		log_debug(logger_debug, "Mate al hilo de quantum >:)");
	}
}

void fin_de_quantum() {
	// 1 milisegundo = 1000 microsegundos
	int usegs = config->quantum_rr * 1000;
	log_debug(logger_debug,
			"Voy a dormir %d ms antes de mandar interrupcion de quantum",
			config->quantum_rr);
	// usando sleep se enviaba la parte entera de: quantum_en_ms/1000 (para pasar a segundos)
	// ej: q = 1500ms => q/1000 = 1,5 seg pero sleep recibia 1 seg
	usleep(usegs);
	log_debug(logger_debug,
			"Termino sleep de quantum -> Enviando interrupcion de quantum");
	enviar_codop(cpu_interrupt_fd, INTERRUPCION_QUANTUM);
}

PCB* recibir_pcb_de_cpu(int cliente_socket) {
	PCB *pcb = recibir_pcb(cliente_socket);
	if (esta_usando_rr) {
		matar_hilo_quantum();
	}
	return pcb;
}

int iniciar_servidor_kernel(char *ip, char *puerto) {
	int server_fd = iniciar_servidor(logger_debug, SERVERNAME, ip, puerto);
	log_info(logger_debug, "Kernel listo para recibir al cliente");
	return server_fd;
}

void inicializar_kernel() {
	t_config *config_file = abrir_configuracion(config_path);
	crear_loggers("kernel", &logger, &logger_debug, config_file);
	config = procesar_config(config_file);

	esta_usando_rr = config->algoritmo_planificacion == RR;

	test_read_config(config);

	// Si el segundo parametro es distinto de 0, el semaforo se comparte entre hilos de un mismo proceso.
	sem_init(&sem_procesos_ready, 1, config->grado_max_multiprogramacion);
	sem_init(&sem_proceso_nuevo, 0, 0);
	pthread_mutex_init(&mutex_ready, NULL);
	sem_init(&planificar, 1, 0);
	sem_init(&cpu_idle, 1, 1);
	sem_init(&sem_impresora, 1, 1);
	sem_init(&sem_disco, 1, 1);
	sem_init(&sem_estructuras_memoria, 1, 0);
	pthread_mutex_init(&mutex_block, NULL);
	pthread_mutex_init(&mutex_new, NULL);

	if (config->algoritmo_planificacion == FEEDBACK) {
		log_debug(logger_debug, "Inicializando para algoritmo FEEDBACK");
		procesosBajaPrioridad = list_create();
		pthread_mutex_init(&mutex_baja_prioridad, NULL);
	}

	procesosNew = list_create();
	procesosReady = list_create();
	procesosExit = list_create();
	procesosBlock = list_create();
}

void destruir_loggers() {
	log_destroy(logger);
	log_destroy(logger_debug);
}

void destruir_listas_y_elementos() {
	list_destroy_and_destroy_elements(procesosNew, (void*) destruir_pcb);
	list_destroy_and_destroy_elements(procesosReady, (void*) destruir_pcb);
	list_destroy_and_destroy_elements(procesosBlock, (void*) destruir_pcb);
	list_destroy_and_destroy_elements(procesosExit, (void*) destruir_pcb);
	list_destroy_and_destroy_elements(procesosBajaPrioridad, (void*) destruir_pcb);
}

void destruir_semaforos() {
	sem_destroy(&planificar);
	sem_destroy(&cpu_idle);
	sem_destroy(&sem_estructuras_memoria);
	sem_destroy(&sem_disco);
	sem_destroy(&sem_impresora);
	sem_destroy(&sem_procesos_ready);
	sem_destroy(&sem_proceso_nuevo);
	pthread_mutex_destroy(&mutex_ready);
	pthread_mutex_destroy(&mutex_new);
	pthread_mutex_destroy(&mutex_block);
	pthread_mutex_destroy(&mutex_baja_prioridad);
}

void liberar_conexiones() {
	liberar_conexion(memoria_fd);
	liberar_conexion(cpu_interrupt_fd);
	liberar_conexion(cpu_dispatch_fd);
}

void terminar_kernel() {
	liberar_configuracion_kernel(config);
	destruir_listas_y_elementos();
	destruir_semaforos();
	liberar_conexiones();
	destruir_loggers();
}
