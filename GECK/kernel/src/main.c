#include "../include/main.h"

t_log* logger;

int main() {
	int server_fd;

	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	server_fd = iniciar_servidor_kernel("127.0.0.1", "4444");

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	int cpu_interrupt_fd = conectar_cpu("127.0.0.1", "8002");
	if(cpu_interrupt_fd == -1) {
		log_info(logger, "No se ha podido conectar con la CPU (interrupt)");
		exit(EXIT_FAILURE);
	}

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	int cpu_dispatch_fd = conectar_cpu("127.0.0.1", "8005");
	if(cpu_dispatch_fd == -1) {
		log_info(logger, "No se ha podido conectar con la CPU (dispatch)");
		exit(EXIT_FAILURE);
	}

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	int memoria_fd = conectar_memoria("127.0.0.1", "8001");
	if(memoria_fd == -1) {
		log_info(logger, "No se ha podido conectar con la Memoria");
		exit(EXIT_FAILURE);
	}

	send_debug(cpu_interrupt_fd);
	send_debug(cpu_dispatch_fd);
	send_debug(memoria_fd);

	while (server_escuchar(SERVERNAME, server_fd));

	return EXIT_SUCCESS;
}

int iniciar_servidor_kernel(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger, "Kernel listo para recibir al cliente");

	return server_fd;
}


