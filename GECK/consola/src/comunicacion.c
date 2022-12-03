#include "../include/comunicacion.h"

extern t_log* logger;
extern t_log* logger_debug;
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
			log_debug(logger_debug,"FIN POR EXIT");
			return 0;

		case OP_PANTALLA: {
			uint32_t valor = recibir_valor(cliente_socket);
			uint32_t reg = recibir_registro(cliente_socket);
			int pid = recibir_pid(cliente_socket);
			
			usleep(tiempo_pantalla * 1000);
			log_info(logger, "%d", valor);

			enviar_codop(cliente_socket, RESPUESTA_PANTALLA);
			enviar_registro(cliente_socket, reg);
			enviar_pid(cliente_socket, pid);
			break;
		}

		case OP_TECLADO: {
			uint32_t reg = recibir_registro(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			uint32_t valor;

			printf("Ingresar valor numerico: ");
			scanf("%u", &valor);
			log_debug(logger_debug, "se leyo el valor: %u", valor);
			
			enviar_codop(cliente_socket, RESPUESTA_TECLADO);
			enviar_valor(cliente_socket, valor);
			enviar_registro(cliente_socket, reg);
			enviar_pid(cliente_socket, pid);
			break;
		}
		case SEGMENTATION_FAULT:
			log_debug(logger_debug, "Recibi un SEG_FAULT. Voy a terminar.");
			return 0;
		case DEBUG:
			log_debug(logger_debug, "Estoy debuggeando!");
			return 1;

		case -1:
			log_error(logger_debug, "El cliente se desconecto. Terminando servidor");
			close(cliente_socket);
			return 0;

		default:
			log_warning(logger_debug,"Operacion desconocida. No quieras meter la pata");
			return 0;
		}
	}
	return 0;
}

