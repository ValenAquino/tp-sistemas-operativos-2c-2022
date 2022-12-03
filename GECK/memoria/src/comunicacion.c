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
		log_trace(logger_debug, "Esperando codigo de operacion [%s]", server_name);
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case HANDSHAKE_MEMORIA_CPU:
			log_trace(logger_debug, "ENTRE A HANDSHAKE_MEMORIA_CPU");
			enviar_valor(cliente_socket, config->entradas_por_tabla);
			enviar_valor(cliente_socket, config->tam_pagina);

			log_debug(logger_debug,
					"ENVIO - HANDSHAKE_MEMORIA_CPU -> entradas_por_tabla_memoria: %d, tam_pagina_memoria: %d",
					config->entradas_por_tabla, config->tam_pagina);
			break;

		case CREAR_ESTRUCTURAS_MEMORIA: {
			log_trace(logger_debug, "ENTRE A CREAR_ESTRUCTURAS_MEMORIA");
			log_debug(logger_debug, "CREAR_ESTRUCTURAS_MEMORIA");
			t_list *tamanios_segmentos =
					recibir_solicitud_crear_estructuras_memoria(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			t_list *indices = crear_indices_tabla_de_paginas(tamanios_segmentos,
					pid);

			enviar_indices_tablas_de_paginas(indices, cliente_socket, pid);
			break;
		}

		case PAGINA_SOLICITADA:
			log_trace(logger_debug, "ENTRE A PAGINA_SOLICITADA");
			int pid = recibir_valor(cliente_socket);
			int segmento_solicitado = recibir_valor(cliente_socket);
			int nro_pag_solicitada = recibir_valor(cliente_socket);
			int index_en_ts_de_ps = recibir_valor(cliente_socket);

			log_debug(logger_debug,
					"Se esta solicitando cargar en memoria el segmento: %d y la pagina: %d para pcb id: %d",
					segmento_solicitado, nro_pag_solicitada, pid);

			pagina_t *pagina_obtenida = obtener_pagina(index_en_ts_de_ps,
					nro_pag_solicitada);

			void *data_leida_de_swap = leer_pagina_entera_de_swap(
					pagina_obtenida, pid, segmento_solicitado,
					nro_pag_solicitada);

			cargar_pagina_entera_en_memoria_principal(pagina_obtenida,
					data_leida_de_swap, segmento_solicitado,
					nro_pag_solicitada);

			enviar_codop(cliente_socket, PAGINA_ENCONTRADA);
			enviar_pid(cliente_socket, pid);
			break;

		case ACCESO_A_MEMORIA: {
			log_trace(logger_debug, "ENTRE A ACCESO_A_MEMORIA");
			dir_t dir = recibir_direccion_parcial(cliente_socket);
			int pid = recibir_pid(cliente_socket);
			int numero_de_marco = obtener_num_marco(dir, pid);

			ejecutar_retardo_memoria("ACCESO A MEMORIA");

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
			log_trace(logger_debug, "ENTRE A LECTURA_MEMORIA");
			dir_t dir_parcial = recibir_direccion_parcial(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			ejecutar_retardo_memoria("LECTURA DE MEMORIA");

			int valor_leido = leer(pid, dir_parcial);
			enviar_codop(cliente_socket, VALOR_LECTURA_MEMORIA);
			enviar_valor(cliente_socket, valor_leido);
			break;
		}

		case ESCRITURA_MEMORIA: {
			log_trace(logger_debug, "ENTRE A ESCRITURA_MEMORIA");
			dir_t dir_parcial = recibir_direccion_parcial(cliente_socket);
			int pid = recibir_pid(cliente_socket);
			int valor_a_escribir = recibir_valor(cliente_socket);

			escribir(pid, dir_parcial, valor_a_escribir);
			ejecutar_retardo_memoria("ESCRITURA DE MEMORIA");

			enviar_codop(cliente_socket, OK_ESCRITURA_MEMORIA);
			break;
		}
		case DEBUG:
			log_debug(logger_debug, "Estoy debuggeando!");
			break;
		case -1:
			log_error(logger_debug,
					"El cliente se desconecto. Terminando servidor");
			close(cliente_socket);
			exit(EXIT_FAILURE);
			return;
		default:
			log_warning(logger_debug,
					"Operacion <%d> desconocida. No quieras meter la pata", cod_op);
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
