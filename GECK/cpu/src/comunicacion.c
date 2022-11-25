#include "../include/comunicacion.h"
extern t_log *logger;
extern t_log *logger_debug;

extern int FLAG_FIN_QUANTUM;
extern int FLAG_PAGE_FAULT;
extern int MARCO_MEMORIA;

extern int entradas_por_tabla_memoria;
extern int tam_pagina_memoria;
extern int tam_max_segmento_memoria;

extern sem_t sem_acceso_memoria;
extern sem_t sem_respuesta_memoria;

int valor_leido;

void manejar_comunicacion_dispatch(void *void_args) {
	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		log_trace(logger_debug, "Esperando codigo de operacion [%s]",
				server_name);
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case DISPATCH_PCB:
			log_trace(logger_debug, "Recibiendo PCB");
			PCB *pcb = recibir_pcb(cliente_socket);

			restaurar_contexto_ejecucion(pcb->registros);
			log_trace(logger_debug, "Enviando PCB al ciclo de instruccion\n");
			ciclo_de_instruccion(pcb, cliente_socket);
			log_trace(logger_debug, "Ciclo de instruccion completado\n");
			break;

		case DEBUG:
			log_debug(logger_debug, "Estoy debuggeando!\n");
			break;
		case -1:
			log_error(logger_debug,
					"El cliente se desconecto. Terminando servidor [%s]",
					server_name);
			// close(cliente_socket);
			// exit(EXIT_FAILURE);
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

void manejar_comunicacion_interrupt(void *void_args) {
	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		log_trace(logger_debug, "Esperando codigo de operacion [%s]",
				server_name);
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case INTERRUPCION_QUANTUM:
			log_debug(logger_debug, "INTERRUPCION POR FIN DE QUANTUM!\n");
			FLAG_FIN_QUANTUM = 1;
			break;
		case DEBUG:
			log_debug(logger_debug, "Estoy debuggeando!\n");
			break;
		case -1:
			log_error(logger_debug,
					"El cliente se desconecto. Terminando servidor [%s]",
					server_name);
			// close(cliente_socket);
			// exit(EXIT_FAILURE);
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

void manejar_comunicacion_memoria(void *void_args) {
	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		log_trace(logger_debug, "Esperando codigo de operacion [%s]",
				server_name);
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case HANDSHAKE_MEMORIA_CPU:
			entradas_por_tabla_memoria = recibir_valor(cliente_socket);
			tam_pagina_memoria = recibir_valor(cliente_socket);

			log_debug(logger_debug,
					"HANDSHAKE_MEMORIA_CPU -> entradas_por_tabla_memoria: %d, tam_pagina_memoria: %d",
					entradas_por_tabla_memoria, tam_pagina_memoria);

			tam_max_segmento_memoria = entradas_por_tabla_memoria * tam_pagina_memoria;
			break;

		case NUMERO_DE_MARCO_MEMORIA: {
			PCB *pcb = recibir_pcb(cliente_socket);
			MARCO_MEMORIA = recibir_valor(cliente_socket);
			log_debug(logger_debug,
					"La memoria me devolvio numero de marco: %d",
					MARCO_MEMORIA);
			sem_post(&sem_acceso_memoria);
			break;
		}
		case PAGE_FAULT_MEMORIA: {
			PCB *pcb = recibir_pcb(cliente_socket);
			log_debug(logger_debug, "La memoria me devolvio un PAGE FAULT");
			FLAG_PAGE_FAULT = 1;
			sem_post(&sem_acceso_memoria);
			break;
		}
		case OK_ESCRITURA_MEMORIA:
			log_debug(logger_debug, "Escritura de memoria ok");
			sem_post(&sem_respuesta_memoria);
			break;
		case VALOR_LECTURA_MEMORIA:
			valor_leido = recibir_valor(cliente_socket);
			log_debug(logger_debug, "Valor leido de memoria: %d", valor_leido);
			sem_post(&sem_respuesta_memoria);
			break;
		case DEBUG:
			log_debug(logger_debug, "Estoy debuggeando!\n");
			break;
		case -1:
			log_error(logger_debug,
					"El cliente se desconecto. Terminando servidor [%s]",
					server_name);
			return;
		default:
			log_warning(logger_debug,
					"Operacion desconocida. No quieras meter la pata. OP CODE recibido: %d", cod_op);
			break;
		}
	}

	log_warning(logger_debug, "El cliente se desconecto de %s server",
			server_name);
	return;
}

int server_escuchar_interrupt(char *server_name, int server_interrupt_fd) {
	return server_escuchar(logger_debug, server_name, server_interrupt_fd,
			manejar_comunicacion_interrupt);
}

int server_escuchar_dispatch(char *server_name, int server_dispatch_fd) {
	return server_escuchar(logger_debug, server_name, server_dispatch_fd,
			manejar_comunicacion_dispatch);
}

