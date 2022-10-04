#include "../include/main.h"

t_log* logger;

int main() {
	int server_fd;

	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	server_fd = iniciar_servidor_kernel("127.0.0.1", "4444");

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	int cpu_interrupt = conectar_cpu("127.0.0.1", "8002");
	if(cpu_interrupt == -1) {
		log_info(logger, "No se ha podido conectar con la CPU (interrupt)");
		exit(EXIT_FAILURE);
	}

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	int cpu_idispatch = conectar_cpu("127.0.0.1", "8005");
	if(cpu_idispatch == -1) {
		log_info(logger, "No se ha podido conectar con la CPU (dispatch)");
		exit(EXIT_FAILURE);
	}

	while (server_escuchar(SERVERNAME, server_fd));

	return EXIT_SUCCESS;
}

int iniciar_servidor_kernel(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger, "Kernel listo para recibir al cliente");

	return server_fd;
}


