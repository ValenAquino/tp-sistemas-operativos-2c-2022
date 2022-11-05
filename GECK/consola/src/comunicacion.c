#include "../include/comunicacion.h"

extern t_log* logger;
extern int tiempo_pantalla;

int manejar_comunicacion(void* void_args) {
	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	//char* server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case FIN_POR_EXIT:
			log_debug(logger,"FIN POR EXIT");
			return 0;

		case OP_PANTALLA: {
			uint32_t valor = recibir_operacion(cliente_socket);
			
			sleep(tiempo_pantalla/1000);
			log_info(logger, "%d", valor);

			enviar_codop(cliente_socket, RESPUESTA_PANTALLA);
			break;
		}

		case OP_TECLADO: {
			uint32_t valor;

			printf("Ingresar valor numerico: ");
			scanf("%u", &valor);
			log_debug(logger, "se leyo el valor: %u", valor);
			
			enviar_codop(cliente_socket, RESPUESTA_TECLADO);
			enviar_codop(cliente_socket, valor);
			
			break;
		}

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

