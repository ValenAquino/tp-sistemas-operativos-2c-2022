
#include "../include/memoria.h"

t_log* logger;
t_configuracion_memoria* config;

int main() {
	iniciar_memoria();

	int memoria_fd = crear_conexion(config->ip_memoria, config->puerto_escucha);

	while (server_escuchar(SERVERNAME, memoria_fd));

	return EXIT_SUCCESS;
}

void iniciar_memoria() {
	logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);
	config = procesar_config("memoria.config");
	test_read_config(config);
}

int crear_conexion(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger, "%s lista para recibir al cliente", SERVERNAME);

	return server_fd;
}

