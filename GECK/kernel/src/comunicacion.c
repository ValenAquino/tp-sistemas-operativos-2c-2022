#include "../include/comunicacion.h"

extern t_log *logger;
extern t_log *logger_debug;

extern t_list *procesosNew;

extern t_configuracion_kernel *config;

extern pthread_mutex_t mutex_new;
extern sem_t cpu_idle;
extern sem_t sem_estructuras_memoria;
extern sem_t sem_procesos_ready;

int proccess_counter = 1;
uint32_t respuesta_teclado;

void manejar_comunicacion(void *void_args) {
	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case ELEMENTOS_CONSOLA: // consola
			PCB *pcb = crear_pcb(cliente_socket);
			log_pcb(pcb);
			pasarANew(pcb);
			break;

		case RESPUESTA_PANTALLA: {  // consola
			reg_cpu reg = recibir_registro(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			log_debug(logger_debug, "Recibo RESPUESTA_PANTALLA. PID: %d", pid);

			op_pantallla(pid, reg);
			break;
		}

		case RESPUESTA_TECLADO: {  // consola
			uint32_t valor = recibir_valor(cliente_socket);
			reg_cpu reg = recibir_registro(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			op_teclado(pid, reg, valor);
			break;
		}

		case FIN_POR_EXIT: {
			PCB *pcb = recibir_pcb_de_cpu(cliente_socket);
			sem_post(&cpu_idle);
			pasarAExit(pcb);
			break;
		}

		case DESALOJO_QUANTUM: {
			PCB *pcb = recibir_pcb_de_cpu(cliente_socket);
			log_info(logger, "PID: <%d> - Desalojado por fin de Quantum", pcb->id);
			sem_post(&cpu_idle);
			pasarAReady(pcb, true);
			break;
		}

		case IO_KERNEL: {
			PCB *pcb = recibir_pcb_de_cpu(cliente_socket);
			uint32_t indice_dispositivo = recibir_valor(cliente_socket);
			sem_post(&cpu_idle);
			manejar_suspension_por(indice_dispositivo, pcb, cliente_socket);
			break;
		}

		case OP_PANTALLA: {
			PCB *pcb = recibir_pcb_de_cpu(cliente_socket);
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
			PCB *pcb = recibir_pcb_de_cpu(cliente_socket);
			reg_cpu reg = recibir_registro(cliente_socket);
			sem_post(&cpu_idle);
			pasarABlock(pcb, TECLADO);

			log_debug(logger_debug, "Recibo OP_TECLADO.");

			enviar_codop(pcb->socket_consola, OP_TECLADO);

			enviar_registro(pcb->socket_consola, reg);
			enviar_pid(pcb->socket_consola, pcb->id);
			break;
		}

		case PAGE_FAULT_CPU: {
			PCB *pcb = recibir_pcb_de_cpu(cliente_socket);
			int segmento_solicitado = recibir_valor(cliente_socket);
			int pagina_solicitada = recibir_valor(cliente_socket);

			log_info(logger,
					"Page Fault PID: <%d> - Segmento: <%d> - Pagina: <%d>",
					pcb->id, segmento_solicitado, pagina_solicitada
			);

			ejecutar_bloqueo_page_fault(pcb, segmento_solicitado, pagina_solicitada);
			sem_post(&cpu_idle);
			break;
		}

		case SEGMENTATION_FAULT: {
			PCB *pcb = recibir_pcb_de_cpu(cliente_socket);
			
			log_info(logger, "PID: <%d> - Finalizado por SEGMENTATION FAULT", pcb->id);
			enviar_codop(pcb->socket_consola, SEGMENTATION_FAULT);
			free(pcb);

			sem_post(&sem_procesos_ready);
			sem_post(&cpu_idle);
			break;
		}

		case RESPUESTA_INDICES_T_PS: { // memoria
			t_list *indices = recibir_indices_tablas_de_paginas(cliente_socket);
			int pid = recibir_pid(cliente_socket);

			// Aca habria que mapear estos indices a los segmentos pero no tenemos el pcb
			// en la comunicacion el kernel envia los tamanios de los segmentos
			// y aca se reciben los indices de las tablas de paginas del proceso.
			// tal vez tenemos que pasar y devolver el pcb?
			log_debug(logger_debug, "RESPUESTA_INDICES_T_PS - RECIBI INDICES");

			void iterar(int *index) {
				log_debug(logger_debug, "RESPUESTA_INDICES_T_PS, indice: %d", *index);
			}

			list_iterate(indices, (void*) iterar);
			PCB *pcb = obtener_proceso_por_pid(pid, procesosNew, mutex_new);

			for (int i = 0; i < list_size(indices); i++) {
				segmento_t *seg = (segmento_t*) malloc(sizeof(segmento_t));

				seg->tamanio_segmento = *((int*) list_get(pcb->tamanios_segmentos, i));
				seg->indice_tablas_paginas = *((int*) list_get(indices, i));

				list_add(pcb->tablaSegmentos, seg);
			}

			sem_post(&sem_estructuras_memoria);
			break;
		}

		case PAGINA_ENCONTRADA: {
			int pid = recibir_pid(cliente_socket);
			log_debug(logger_debug, "Se cargo en memoria principal la pagina solicitada para el pid: %d", pid);
			mover_proceso_block_ready_by_pid(pid);
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
			close(cliente_socket);
			exit(EXIT_FAILURE);
			return;

		default:
			log_warning(logger_debug, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	log_warning(logger_debug, "El cliente se desconecto de %s server", server_name);
	return;
}

int server_kernel(char *server_name, int server_socket) {
	return server_escuchar(logger_debug, server_name, server_socket, manejar_comunicacion);
}

int conectar_con(char *servername, char *ip, char *puerto) {
	log_info(logger_debug, "Iniciando conexion con %s - Puerto: %s - IP: %s", ip, puerto, servername);

	int file_descriptor = crear_conexion(ip, puerto);

	if (file_descriptor == -1) {
		log_info(logger, "No se ha podido conectar %s", servername);
		exit(EXIT_FAILURE);
	}

	return file_descriptor;
}


reg_cpu procesar_reg(char *reg) {
	if(strcmp("AX", reg) == 0)
		return AX;

	if(strcmp("BX", reg) == 0)
		return BX;

	if(strcmp("CX", reg) == 0)
		return CX;

	if(strcmp("DX", reg) == 0)
		return DX;

	log_error(logger_debug, "el registro '%s' es incorrecto", reg);
	exit(EXIT_FAILURE);
}

int procesar_dispositivo(char *disp) {
	int indice_dispositivo = -1; // -1 para poder iterar dentro de la nested function

	bool mismo_nombre(void *arg) {
		indice_dispositivo++;
		ts_dispositivo *dispositivo = (ts_dispositivo *) arg;
		return strcmp(dispositivo->nombre, disp) == 0;
	}

	ts_dispositivo *dispositivo = list_find(config->dispositivos, (void*) mismo_nombre);

	if(dispositivo == NULL) {
		log_error(logger_debug, "el dispositivo '%s' es incorrecto", disp);
		exit(EXIT_FAILURE);
	}

	return indice_dispositivo;
}

int procesar_parametro2_io(int disp, char* parametro2) {
	if ((disp == TECLADO) || (disp == PANTALLA))
		return procesar_reg(parametro2);
	else
		return atoi(parametro2);
}

ts_ins *convertir_inst_consola_a_inst_kernel(t_ins enum_ins, char *parametro1, char *parametro2) {
	ts_ins *inst = malloc(sizeof(ts_ins));
	inst->name = enum_ins;

	switch (enum_ins) {
	case SET:
			inst->param1 = procesar_reg(parametro1);
			inst->param2 = atoi(parametro2);
			break;
	case ADD:
			inst->param1 = procesar_reg(parametro1);
			inst->param2 = procesar_reg(parametro2);
			break;
	case MOV_IN:
			inst->param1 = procesar_reg(parametro1);
			inst->param2 = atoi(parametro2);
			break;
	case MOV_OUT:
			inst->param1 = atoi(parametro1);
			inst->param2 = procesar_reg(parametro2);
			break;
	case EXIT:
			inst->param1 = -1;
			inst->param2 = -1;
			break;
	case IO:
			inst->param1 = procesar_dispositivo(parametro1);
			inst->param2 = procesar_parametro2_io(inst->param1, parametro2);
			break;
	default:
			log_error(logger_debug, "ins %d no reconocida", enum_ins);
			exit(EXIT_FAILURE);
			break;
	}

	free(parametro1);
	free(parametro2);

	return inst;
}

t_list *crear_lista_instrucciones(t_list *lista_ins_consola) {
	t_list *lista_ins = list_create();

	for(int i=0; i< list_size(lista_ins_consola); i++) {

		ts_ins_consola *inst_consola = list_get(lista_ins_consola, i);

		ts_ins *ins = convertir_inst_consola_a_inst_kernel(
			inst_consola->name,
			inst_consola->param1,
			inst_consola->param2
		);

		list_add(lista_ins, ins);
		free(inst_consola);
	}

	return lista_ins;
}

PCB* crear_pcb(int cliente_socket) {
	t_list *listas = recibir_paquete(cliente_socket);

	void *ins = list_get(listas, 0);
	void *seg = list_get(listas, 1);

	t_list *lista_ins_consola = deserializar_lista_ins_consola(ins);
	t_list *lista_ins = crear_lista_instrucciones(lista_ins_consola);
	t_list *lista_segm = deserializar_lista_tamanios_segm(seg);

	PCB *pcb = nuevoPcb(proccess_counter, cliente_socket, lista_ins, lista_segm);
	proccess_counter++;

	list_destroy(lista_ins_consola);

	return pcb;
}
