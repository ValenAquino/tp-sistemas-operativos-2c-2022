
#include "../include/cpu.h"

t_log* logger;

int main() {
	int server_interrupt_fd;
	int server_dispatch_fd;

	logger = log_create("cpu.log", "CPU", 1, LOG_LEVEL_DEBUG);

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	server_interrupt_fd = crear_servidor("127.0.0.1", "8005", INTERRUPT_SERVER_NAME);
	server_dispatch_fd = crear_servidor("127.0.0.1", "8002", DISPATCH_SERVER_NAME);

	while (server_escuchar(INTERRUPT_SERVER_NAME, server_interrupt_fd) &&
			server_escuchar(DISPATCH_SERVER_NAME, server_dispatch_fd));

	return EXIT_SUCCESS;
}

int crear_servidor(char* ip, char* puerto, char* server_name) {
	int server_fd = iniciar_servidor(logger, server_name, ip, puerto);

	log_info(logger, "%s listo para recibir al cliente", server_name);

	return server_fd;
}

