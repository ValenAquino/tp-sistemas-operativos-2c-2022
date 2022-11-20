#include "../include/memoria.h"

t_log* logger;
t_log* logger_debug;
t_configuracion_memoria* config;
char* config_path;

int main(int argc, char** argv) {
	if (argc < 2) {
		config_path = "memoria.config";
	} else {
		config_path = argv[1];
	}

	iniciar_memoria();
	log_debug(logger_debug, "config: %s", config_path);
	int memoria_fd = crear_conexion(config->ip_memoria, config->puerto_escucha);

	while (server_memoria(SERVERNAME, memoria_fd));

	return EXIT_SUCCESS;
}

void iniciar_memoria() {
	t_config *config_file = abrir_configuracion(config_path);
	crear_loggers("memoria", &logger, &logger_debug, config_file);
	config = procesar_config(config_file);
	test_read_config(config);
}

int crear_conexion(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger_debug, "%s lista para recibir al cliente", SERVERNAME);

	return server_fd;
}
