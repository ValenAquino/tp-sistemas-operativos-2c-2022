#include "../include/comunicacion.h"

extern t_log *logger_debug;
extern t_configuracion_memoria *config;

void manejar_comunicacion(void *void_args) {
	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case HANDSHAKE_MEMORIA_CPU:
			enviar_codop(cliente_socket, HANDSHAKE_MEMORIA_CPU);
			enviar_valor(cliente_socket, config->entradas_por_tabla);
			enviar_valor(cliente_socket, config->tam_pagina);

			log_debug(logger_debug,
					"ENVIO - HANDSHAKE_MEMORIA_CPU -> entradas_por_tabla_memoria: %d, tam_pagina_memoria: %d",
					config->entradas_por_tabla, config->tam_pagina);
			break;

		case CREAR_ESTRUCTURAS_MEMORIA: {
			log_debug(logger_debug, "CREAR_ESTRUCTURAS_MEMORIA");
			t_list *tamanios_segmentos =
					recibir_solicitud_crear_estructuras_memoria(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			t_list *indices = crear_indices_tabla_de_paginas(
					tamanios_segmentos);

			enviar_indices_tablas_de_paginas(indices, cliente_socket, pid);
			break;
		}
		case PAGINA_SOLICITADA:
			int pid = recibir_valor(cliente_socket);
			int segmento_solicitado = recibir_valor(cliente_socket);
			int pagina_solicitada = recibir_valor(cliente_socket);

			log_debug(logger_debug,
					"Se esta solicitando el sector: %d y la pagina: %d para pcb id: %d",
					segmento_solicitado, pagina_solicitada, pid);

			//TODO: IMPLEMENTAR SOLICITUD A SWAP.

			// REVISAR: Este envio esta ok? Es necesario? Si es necesario quizas habria
			// que llamarlo cuando cuando swap responda ok.
			enviar_codop(cliente_socket, PAGINA_ENCONTRADA);
			enviar_valor(cliente_socket, pid);
			enviar_valor(cliente_socket, 45); // valor pagina encontrada.
			break;
		case ACCESO_A_MEMORIA: {
			dir_t dir = recibir_direccion_parcial(cliente_socket);

			int numero_de_marco = obtener_num_marco(dir);

			if (numero_de_marco == PAGE_FAULT_ERROR) {
				log_debug(logger_debug,
						"No se encontro el numero de marco. Enviando PAGE FAULT a CPU");
				enviar_codop(cliente_socket, PAGE_FAULT_MEMORIA);
			} else {
				log_debug(logger_debug,
						"Se obtuvo el marco de memoria: %d. Enviandolo a CPU.",
						numero_de_marco);
				enviar_codop(cliente_socket, NUMERO_DE_MARCO_MEMORIA);
				enviar_valor(cliente_socket, numero_de_marco);
			}
			break;
		}
		case LECTURA_MEMORIA: {
			int marco = recibir_valor(cliente_socket);
			int offset = recibir_valor(cliente_socket);

			log_debug(logger_debug,
					"Voy a leer el marco: %d y devolver su contenido", marco);

			int valor_leido = leer(marco);
			enviar_codop(cliente_socket, VALOR_LECTURA_MEMORIA);
			enviar_valor(cliente_socket, valor_leido);

			break;
		}
		case ESCRITURA_MEMORIA: {
			int marco = recibir_valor(cliente_socket);
			int valor_a_escribir = recibir_valor(cliente_socket);

			log_debug(logger_debug,
					"Voy a escribir el marco: %d con el siguiente valor: %d",
					marco, valor_a_escribir);

			escribir(marco, valor_a_escribir);
			enviar_codop(cliente_socket, OK_ESCRITURA_MEMORIA);

			break;
		}
		case DEBUG:
			log_debug(logger_debug, "Estoy debuggeando!");
			break;
		case -1:
			log_error(logger_debug,
					"El cliente se desconecto. Terminando servidor");
			return;
		default:
			log_warning(logger_debug,
					"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	log_warning(logger_debug, "El cliente se desconecto de %s server",
			server_name);
	return;
}

int server_memoria(char *server_name, int server_socket) {
	return server_escuchar(logger_debug, server_name, server_socket,
			manejar_comunicacion);
}
