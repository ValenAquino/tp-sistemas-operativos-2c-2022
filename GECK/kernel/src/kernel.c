#include "../include/kernel.h"

t_log* logger;
t_list* procesosNew;
t_list* procesosReady;
t_list* procesosExit;
t_configuracion_kernel *config;
	
int cpu_dispatch_fd; // PROBANDO ENVIOS A CPU
sem_t sem_procesos_ready;
sem_t sem_proceso_nuevo;
sem_t mutex_ready;
sem_t planificar;

int main() {
	inicializar_kernel();

	int server_fd = iniciar_servidor_kernel(config->ip_kernel, config->puerto_kernel);

	int cpu_interrupt_fd = conectar_con("CPU (interrupt)", config->ip_cpu, config->puerto_cpu_interrupt);
	hilo_escucha_dispatch();
	int memoria_fd = conectar_con("Memoria", config->ip_memoria, config->puerto_memoria);

	handshake_dispatch(cpu_dispatch_fd, config->tiempos_io);
	
	send_debug(cpu_interrupt_fd);
	send_debug(cpu_dispatch_fd);
	send_debug(memoria_fd);

	hilo_planificador_largo_plazo();

	while (server_escuchar(SERVERNAME, server_fd));
	return EXIT_SUCCESS;
}

void hilo_planificador_largo_plazo() {
	pthread_t hilo;

	pthread_create(&hilo, NULL, (void*) planificador_largo_plazo, NULL);
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

int iniciar_servidor_kernel(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger, "Kernel listo para recibir al cliente");

	return server_fd;
}

void inicializar_kernel() {
	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_TRACE);
	config = procesar_config("kernel.config");

	sem_init(&sem_procesos_ready, 1, config->grado_max_multiprogramacion); // Si el segundo parametro es distinto de 0, el semaforo se comparte entre hilos de un mismo proceso.
	sem_init(&sem_proceso_nuevo, 0, 0);
	sem_init(&mutex_ready, 1, 1);
	sem_init(&planificar, 1, 0);

	procesosNew = list_create();
	procesosReady = list_create();
	procesosExit = list_create();
}