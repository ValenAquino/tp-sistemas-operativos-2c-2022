#include "../include/cpu.h"

t_log* logger;
t_log* logger_debug;

t_configuracion_cpu *config;
int FLAG_FIN_QUANTUM;
int FLAG_PAGE_FAULT;
int MARCO_MEMORIA = -1;

t_list* tiempos_io;
uint32_t REG_AX;
uint32_t REG_BX;
uint32_t REG_CX;
uint32_t REG_DX;

sem_t sem_acceso_memoria;
sem_t sem_respuesta_memoria;
int memoria_fd;
char* config_path;
int main(int argc, char** argv) {
	if (argc < 2) {
		config_path = "cpu.config";
	} else {
		config_path = argv[1];
	}

	iniciar_cpu();
	log_debug(logger_debug, "config: %s", config_path);

	hilo_memoria();
	send_debug(memoria_fd);

	int server_interrupt_fd = crear_servidor(config->ip_cpu, config->puerto_escucha_interrupt, INTERRUPT_SERVER_NAME);
	int server_dispatch_fd = crear_servidor(config->ip_cpu, config->puerto_escucha_dispatch, DISPATCH_SERVER_NAME);

	while (server_escuchar_interrupt(INTERRUPT_SERVER_NAME, server_interrupt_fd) &&
			server_escuchar_dispatch(DISPATCH_SERVER_NAME, server_dispatch_fd));

	return EXIT_SUCCESS;
}

void hilo_memoria() {
	pthread_t hilo;

	memoria_fd = conectar_con("Memoria", config->ip_memoria, config->puerto_memoria);

	t_manejar_conexion_args* args = malloc(sizeof(t_manejar_conexion_args));
	args->fd = memoria_fd;
	args->server_name = "Memoria";

	pthread_create(&hilo, NULL, (void*) manejar_comunicacion_memoria, (void*) args);
	pthread_detach(hilo);
}

void iniciar_cpu() {
	t_config *config_file = abrir_configuracion(config_path);
	crear_loggers("cpu", &logger, &logger_debug, config_file);
	config = procesar_config(config_file);
	test_read_config(config);

	sem_init(&sem_acceso_memoria, 0, 0);
	sem_init(&sem_respuesta_memoria, 0, 0);
}

int crear_servidor(char* ip, char* puerto, char* server_name) {
	int server_fd = iniciar_servidor(logger, server_name, ip, puerto);

	log_info(logger_debug, "%s listo para recibir al cliente", server_name);

	return server_fd;
}

int conectar_con(char *servername, char *ip, char *puerto) {
	log_info(logger_debug, "Iniciando conexion con %s - Puerto: %s - IP: %s", ip, puerto, servername);

	int file_descriptor = crear_conexion(ip, puerto);

	if(file_descriptor == -1) {
		log_info(logger, "No se ha podido conectar %s", servername);
		exit(EXIT_FAILURE);
	}

	return file_descriptor;
}

void leer_de_memoria(int marco) {
	enviar_codop(memoria_fd, LECTURA_MEMORIA);
	enviar_valor(memoria_fd, marco);
}

void escribir_en_memoria(int marco, int valor) {
	enviar_codop(memoria_fd, ESCRITURA_MEMORIA);
	enviar_valor(memoria_fd, marco);
	enviar_valor(memoria_fd, valor);
}
