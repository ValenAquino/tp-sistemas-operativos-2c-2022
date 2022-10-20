
#include "../include/memoria.h"

t_log* logger;

int main() {
	int memoria_fd;

	logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	memoria_fd = crear_conexion("127.0.0.1", "8001");

	while (server_escuchar(SERVERNAME, memoria_fd));

	return EXIT_SUCCESS;
}

int crear_conexion(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger, "%s lista para recibir al cliente", SERVERNAME);

	return server_fd;
}

