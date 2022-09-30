#include "../include/main.h"

t_log* logger;

int main(void) {
	return iniciar_servidor_kernel();
}

int iniciar_servidor_kernel(void) {
	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);

	// TODO: Leer IP y PUERTO desde un archivo de configuracion.
	int server_fd = iniciar_servidor("127.0.0.1", "4444");
	log_info(logger, "Kernel listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		// Si cod_op es -1 habria que ver como llega a la funcion manejar_comunicacion.
		// En el caso de que haya algun problema con eso, deberiamos manejarlo aca.
		if (manejar_comunicacion(cod_op) == -1) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
