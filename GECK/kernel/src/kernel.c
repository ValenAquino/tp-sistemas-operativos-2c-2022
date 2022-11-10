#include "../include/kernel.h"

t_log* logger;
t_log* logger_debug;

t_configuracion_kernel *config;

int cpu_dispatch_fd;
int cpu_interrupt_fd;

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

	// TODO: Chequear el momento indicado para empezar a correrlo
	// Talvez solo si la config tiene este algoritmo para planificar.
	hilo_quantum_rr();

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

void hilo_quantum_rr() {
	pthread_t hilo;

	pthread_create(&hilo, NULL, (void*) fin_de_quantum, (void*) NULL);
	pthread_detach(hilo);
}

void fin_de_quantum() {
	while(1) {
		sleep(config->quantum_rr / 1000);
		enviar_codop(cpu_interrupt_fd, INTERRUPCION_QUANTUM);
	}
}

PCB* recibir_pcb_de_cpu(int cliente_socket) {
	PCB* pcb = recibir_pcb(cliente_socket);
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
