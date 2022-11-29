#include "../include/comunicacion.h"
extern t_log *logger_debug;

extern t_configuracion_cpu* config;

extern int FLAG_FIN_QUANTUM;

extern int entradas_por_tabla_memoria;
extern int tam_pagina_memoria;
extern int tam_max_segmento_memoria;

extern int memoria_fd;

extern pthread_mutex_t mutex_comunicacion_memoria;

void manejar_comunicacion_dispatch(void *void_args) {
	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		log_trace(logger_debug, "Esperando codigo de operacion [%s]", server_name);
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case DISPATCH_PCB:
			log_trace(logger_debug, "Recibiendo PCB");
			PCB *pcb = recibir_pcb(cliente_socket);

			restaurar_contexto_ejecucion(pcb->registros);
			ciclo_de_instruccion(pcb, cliente_socket);
			log_trace(logger_debug, "Ciclo de instruccion completado\n");
			break;

		case DEBUG:
			log_debug(logger_debug, "Estoy debuggeando!\n");
			break;

		case -1:
			log_error(
				logger_debug, "El cliente se desconecto. Terminando servidor [%s]", server_name);

			close(cliente_socket);
			exit(EXIT_FAILURE);
			return;

		default:
			log_warning(
				logger_debug, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	log_warning(logger_debug, "El cliente se desconecto de %s server", server_name);
	return;
}

void manejar_comunicacion_interrupt(void *void_args) {
	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		log_trace(logger_debug, "Esperando codigo de operacion [%s]", server_name);
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
			log_error( logger_debug, "El cliente se desconecto. Terminando servidor [%s]", server_name);
			 close(cliente_socket);
			 exit(EXIT_FAILURE); // con esto no se liberan estructuras
			return;

		default:
			log_warning(logger_debug, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	log_warning(logger_debug, "El cliente se desconecto de %s server", server_name);
	return;
}

void inciar_comunicacion_con_memoria() {
	memoria_fd = conectar_con("Memoria", config->ip_memoria, config->puerto_memoria);

	pthread_mutex_lock(&mutex_comunicacion_memoria);
	enviar_codop(memoria_fd, HANDSHAKE_MEMORIA_CPU); // Se recibe en dispatch (comunicacin.c)
	entradas_por_tabla_memoria = recibir_valor(memoria_fd);
	tam_pagina_memoria = recibir_valor(memoria_fd);
	tam_max_segmento_memoria = entradas_por_tabla_memoria * tam_pagina_memoria;
	pthread_mutex_unlock(&mutex_comunicacion_memoria);

	log_debug(
		logger_debug,
		"HANDSHAKE_MEMORIA_CPU -> entradas_por_tabla_memoria: %d, tam_pagina_memoria: %d",
		entradas_por_tabla_memoria, tam_pagina_memoria);
}


//void manejar_comunicacion_memoria(void *void_args) {
//	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
//	int cliente_socket = args->fd;
//	char *server_name = args->server_name;
//	free(args);
//
//	// Mientras la conexion este abierta
//	while (cliente_socket != -1) {
//		log_trace(logger_debug, "Esperando codigo de operacion [%s]", server_name);
//		int cod_op = recibir_operacion(cliente_socket);
//
//		switch (cod_op) {
//		case HANDSHAKE_MEMORIA_CPU:
//			entradas_por_tabla_memoria = recibir_valor(cliente_socket);
//			tam_pagina_memoria = recibir_valor(cliente_socket);
//			tam_max_segmento_memoria = entradas_por_tabla_memoria * tam_pagina_memoria;
//
//			log_debug(
//				logger_debug,
//				"HANDSHAKE_MEMORIA_CPU -> entradas_por_tabla_memoria: %d, tam_pagina_memoria: %d",
//				entradas_por_tabla_memoria, tam_pagina_memoria);
//
//		break;

//		case NUMERO_DE_MARCO_MEMORIA: {
//			MARCO_MEMORIA = recibir_valor(cliente_socket);
//			log_debug(logger_debug,
//					"La memoria me devolvio numero de marco: %d",
//					MARCO_MEMORIA);
//			// El ingreso a tlb se hace despues del wait del sem_acceso_memoria
//			sem_post(&sem_acceso_memoria);
//			break;
//		}
//		case PAGE_FAULT_MEMORIA: {
//			log_debug(logger_debug, "La memoria me devolvio un PAGE FAULT");
//			MARCO_MEMORIA = PAGE_FAULT_ERROR;
//			sem_post(&sem_acceso_memoria);
//			break;
//		}

//		case OK_ESCRITURA_MEMORIA:
//			log_debug(logger_debug, "Escritura de memoria ok");
//			sem_post(&sem_respuesta_memoria);
//			break;

//		case VALOR_LECTURA_MEMORIA:
//			valor_leido = recibir_valor(cliente_socket);
//			log_debug(logger_debug, "Valor leido de memoria: %d", valor_leido);
//			sem_post(&sem_respuesta_memoria);
//			break;
//
//		case DEBUG:
//			log_debug(logger_debug, "Estoy debuggeando!\n");
//			break;
//
//		case -1:
//			log_error(logger_debug,
//					"El cliente se desconecto. Terminando servidor [%s]",
//					server_name);
//			return;
//
//		default:
//			log_warning(logger_debug,
//					"Operacion desconocida. No quieras meter la pata. OP CODE recibido: %d", cod_op);
//			break;
//		}
//	}
//
//	log_warning(logger_debug, "El cliente se desconecto de %s server", server_name);
//	return;
//}

int server_escuchar_interrupt(char *server_name, int server_interrupt_fd) {
	return server_escuchar(logger_debug, server_name, server_interrupt_fd,
			manejar_comunicacion_interrupt);
}

int server_escuchar_dispatch(char *server_name, int server_dispatch_fd) {
	return server_escuchar(logger_debug, server_name, server_dispatch_fd,
			manejar_comunicacion_dispatch);
}

int conectar_con(char *servername, char *ip, char *puerto) {
	log_info(
		logger_debug,
		"Iniciando conexion con %s - Puerto: %s - IP: %s",
		servername, puerto, ip);

	int file_descriptor = crear_conexion(ip, puerto);

	if (file_descriptor == -1) {
		log_info(logger, "No se ha podido conectar %s", servername);
		exit(EXIT_FAILURE);
	}

	return file_descriptor;
}

