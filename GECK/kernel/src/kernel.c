#include "../include/kernel.h"

t_log* logger;
t_log* logger_debug;

t_configuracion_kernel *config;

int cpu_dispatch_fd;
int cpu_interrupt_fd;

bool esta_usando_rr;
bool volvio_pcb = false; // Tener en cuenta para IO/EXIT/PAGE_FAULT, etc.

t_list* procesosNew;
t_list* procesosReady;
t_list* procesosBlock;
t_list* procesosExit;
	
sem_t sem_procesos_ready;
sem_t sem_proceso_nuevo;

sem_t mutex_ready;
sem_t mutex_block;

sem_t planificar;
sem_t cpu_idle;

pthread_t hilo_quantum;

int main() {
	inicializar_kernel();

	int server_fd = iniciar_servidor_kernel(config->ip_kernel, config->puerto_kernel);

	hilo_cpu_interrupt();
	hilo_escucha_dispatch();
	int memoria_fd = conectar_con("Memoria", config->ip_memoria, config->puerto_memoria);
	
	send_debug(cpu_interrupt_fd);
	send_debug(cpu_dispatch_fd);
	send_debug(memoria_fd);

	hilo_planificador_largo_plazo();
	hilo_planificador_corto_plazo();

	while (server_kernel(SERVERNAME, server_fd));
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

void hilo_cpu_interrupt() {
	pthread_t hilo;

	cpu_interrupt_fd = conectar_con(
		"CPU (interrupt)", config->ip_cpu, config->puerto_cpu_interrupt
	);

	t_manejar_conexion_args* args = malloc(sizeof(t_manejar_conexion_args));
	args->fd = cpu_interrupt_fd;
	args->server_name = "CPU - Interrupt";

	pthread_create(&hilo, NULL, (void*) manejar_comunicacion, (void*) args);
	pthread_detach(hilo);
}

void hilo_escucha_dispatch() {
	pthread_t hilo;

	cpu_dispatch_fd = conectar_con(
		"CPU (dispatch)", config->ip_cpu, config->puerto_cpu_dispatch
	);

	t_manejar_conexion_args* args = malloc(sizeof(t_manejar_conexion_args));
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
		log_error(logger_debug, "Error intentando matar al hilo de quantum :`(");
	} else {
		log_debug(logger_debug, "Mate al hilo de quantum >:)");
	}
}

void fin_de_quantum() {
	while(1) {
		log_debug(logger_debug, "Voy a dormir %d segundos antes de mandar interrupcion de quantum", config->quantum_rr / 1000);
		sleep(config->quantum_rr / 1000);
		log_debug(logger_debug, "Termino sleep de quantum -> Enviando interrupcion de quantum");
		enviar_codop(cpu_interrupt_fd, INTERRUPCION_QUANTUM);
	}
}

PCB* recibir_pcb_de_cpu(int cliente_socket) {
	log_debug(logger_debug, "Recibi un pcb de CPU");
	PCB* pcb = recibir_pcb(cliente_socket);
	matar_hilo_quantum();
	sem_post(&cpu_idle);
	return pcb;
}


int iniciar_servidor_kernel(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger_debug, SERVERNAME, ip, puerto);
	log_info(logger_debug, "Kernel listo para recibir al cliente");
	return server_fd;
}

void inicializar_kernel() {
	t_config *config_file = abrir_configuracion("kernel.config");
	crear_loggers("kernel", &logger, &logger_debug, config_file);
	config = procesar_config(config_file);

	esta_usando_rr = config->algoritmo_planificacion == RR;

	test_read_config(config);

	// Si el segundo parametro es distinto de 0, el semaforo se comparte entre hilos de un mismo proceso.
	sem_init(&sem_procesos_ready, 1, config->grado_max_multiprogramacion);
	sem_init(&sem_proceso_nuevo, 0, 0);
	sem_init(&mutex_ready, 1, 1);
	sem_init(&planificar, 1, 0);
	sem_init(&cpu_idle, 1, 1);
	sem_init(&mutex_block, 1, 1);

	procesosNew = list_create();
	procesosReady = list_create();
	procesosExit = list_create();
	procesosBlock = list_create();
}
