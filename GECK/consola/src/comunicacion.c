
#include "../include/comunicacion.h"

extern t_log* logger;

int manejar_comunicacion(void* void_args) {
	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char* server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case FIN_POR_EXIT:
			log_debug(logger,"FIN POR EXIT");
			return 0;
		case DEBUG:
			log_debug(logger, "Estoy debuggeando!");
			return 1;
		case -1:
			log_error(logger, "El cliente se desconecto. Terminando servidor");

			return 0;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			return 0;
		}
	}
	return 0;
}

