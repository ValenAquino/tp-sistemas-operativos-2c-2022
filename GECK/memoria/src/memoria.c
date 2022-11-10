#include "../include/memoria.h"

t_log* logger;
t_log* logger_debug;
t_configuracion_memoria* config;

int main() {
	iniciar_memoria();
	int memoria_fd = crear_conexion(config->ip_memoria, config->puerto_escucha);

	while (server_escuchar(SERVERNAME, memoria_fd));

	return EXIT_SUCCESS;
}

void iniciar_memoria() {
	t_config *config_file = abrir_configuracion("memoria.config");
	crear_loggers("memoria", &logger, &logger_debug, config_file);
	config = procesar_config(config_file);
	test_read_config(config);
}

int crear_conexion(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger_debug, "%s lista para recibir al cliente", SERVERNAME);

	return server_fd;
}
