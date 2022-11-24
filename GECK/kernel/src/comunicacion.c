#include "../include/comunicacion.h"

extern t_log* logger;
extern t_log* logger_debug;

extern t_list* procesosNew;

extern pthread_mutex_t mutex_new;
extern sem_t cpu_idle;
extern sem_t sem_estructuras_memoria;

int proccess_counter = 1;
uint32_t respuesta_teclado;

void manejar_comunicacion(void* void_args) {
	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char* server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case ELEMENTOS_CONSOLA: // unico case en el que entra consola
			PCB *pcb = crear_pcb(cliente_socket);
			pasarANew(pcb);
			break;
	
		case FIN_POR_EXIT: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			sem_post(&cpu_idle);
			pasarAExit(pcb);
			break;
		}

		case DESALOJO_QUANTUM: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			log_info(logger, "PID: <%d> - Desalojado por fin de Quantum", pcb->id);
			sem_post(&cpu_idle);
			pasarAReady(pcb, true); // Revisar si tiene que pasar directo a ready o si tiene que bloquearse (ver grado multiprogramacion)
			break;
		}

		case OP_DISCO: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			sem_post(&cpu_idle);
			manejar_suspension_por(DISCO, pcb, cliente_socket);
			break;
		}

		case OP_IMPRESORA: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			sem_post(&cpu_idle);
			manejar_suspension_por(IMPRESORA, pcb, cliente_socket);
			break;
		}

		case OP_PANTALLA: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			uint32_t reg = recibir_registro(cliente_socket);
			sem_post(&cpu_idle);
			pasarABlock(pcb, PANTALLA);

			enviar_codop(pcb->socket_consola, OP_PANTALLA);
			enviar_valor(pcb->socket_consola, pcb->registros[reg]);
			enviar_registro(pcb->socket_consola, reg);
			enviar_pid(pcb->socket_consola, pcb->id);
			break;
		}

		case OP_TECLADO: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			reg_cpu reg = recibir_registro(cliente_socket);
			sem_post(&cpu_idle);
			pasarABlock(pcb, TECLADO);

			log_debug(logger_debug, "Recibo OP_TECLADO.");

			enviar_codop(pcb->socket_consola, OP_TECLADO);
			
			enviar_registro(pcb->socket_consola, reg);
			enviar_pid(pcb->socket_consola, pcb->id);
			break;
		}

		case RESPUESTA_PANTALLA: {
			reg_cpu reg = recibir_registro(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			log_debug(logger_debug, "Recibo RESPUESTA_PANTALLA. PID: %d", pid);


			op_pantallla(pid, reg);
			break;
	    }

		case RESPUESTA_TECLADO: {
			uint32_t valor = recibir_valor(cliente_socket);
			reg_cpu reg = recibir_registro(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			op_teclado(pid, reg, valor);
			break;
		}

		case RESPUESTA_INDICES_T_PS: {
			t_list* indices = recibir_indices_tablas_de_paginas(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			// Aca habria que mapear estos indices a los segmentos pero no tenemos el pcb
			// en la comunicacion el kernel envia los tamanios de los segmentos
			// y aca se reciben los indices de las tablas de paginas del proceso.
			// tal vez tenemos que pasar y devolver el pcb?
			log_debug(logger_debug, "RESPUESTA_INDICES_T_PS - RECIBI INDICES");
			void iterar(int* index) {
				log_debug(logger_debug, "RESPUESTA_INDICES_T_PS, indice: %d", *index);
			}

			list_iterate(indices, (void*) iterar);
			PCB* pcb = obtener_proceso_por_pid(pid, procesosNew, mutex_new);

			for(int i = 0; i < list_size(indices); i++) {
				segmento_t *seg = (segmento_t *) malloc(sizeof(segmento_t));

				seg->tamanio_segmento = *((int *) list_get(pcb->tamanios_segmentos, i));
				seg->num_pagina = *((int *) list_get(indices, i));

				list_add(pcb->tablaSegmentos, seg);
			}

			sem_post(&sem_estructuras_memoria);
			break;
		}
		case PAGE_FAULT_CPU: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			int segmento_solicitado = recibir_valor(cliente_socket);
			int pagina_solicitada = recibir_valor(cliente_socket);

			// TODO: Aca tiene que venir el segmento tambien?
			log_info(logger, "Page Fault PID: <%d> - Segmento: <%d> - Pagina: <%d>", pcb->id, segmento_solicitado, pagina_solicitada);

			ejecutar_bloqueo_page_fault(pcb, segmento_solicitado, pagina_solicitada);
			break;
		}
		case PAGINA_ENCONTRADA: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			log_debug(logger_debug, "Memoria encontro la pagina solicitada!");
//			pasarAReady(pcb);

			break;
		}
		case DEBUG:
			log_debug(logger_debug, "Estoy debuggeando!");
			break;

		case DESCONEXION_CONTROLADA:
			log_info(logger_debug, "El cliente se desconecto de manera esperada");
			close(cliente_socket);
			return;

		case -1:
			log_error(logger_debug, "El cliente se desconecto. Terminando servidor");
			return;

		default:
			log_warning(logger_debug,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	log_warning(logger_debug, "El cliente se desconecto de %s server", server_name);
	return;
}

int server_kernel(char* server_name, int server_socket) {
    return server_escuchar(logger_debug, server_name, server_socket, manejar_comunicacion);
}

int conectar_con(char *servername, char *ip, char *puerto) {
	log_info(logger_debug, "Iniciando conexion con %s - Puerto: %s - IP: %s", ip, puerto, servername);

	int file_descriptor = crear_conexion(ip, puerto);

	if(file_descriptor == -1) {
		log_info(logger, "No se ha podido conectar %s", servername);
		exit(EXIT_FAILURE);
	}

	return file_descriptor;
}

PCB* crear_pcb(int cliente_socket) {
	t_list *listas = recibir_paquete(cliente_socket);

	void* ins = list_get(listas, 0);
	void* seg = list_get(listas, 1);

	t_list *lista_ins = deserializar_lista_inst(ins);
	t_list *lista_segm = deserializar_lista_tamanios_segm(seg);

	PCB *pcb = nuevoPcb(proccess_counter, cliente_socket, lista_ins, lista_segm);
	
	proccess_counter++;

	return pcb;
}
