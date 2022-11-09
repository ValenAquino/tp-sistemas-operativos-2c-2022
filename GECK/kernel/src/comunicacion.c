#include "../include/comunicacion.h"

extern t_log* logger;
extern t_configuracion_kernel* config;
int proccess_counter = 0;

extern sem_t pantalla_consola;
extern sem_t teclado_consola;
extern sem_t planificar;

uint32_t respuesta_teclado;


void manejar_op_teclado(void* args) {
	t_manejar_block_consola* datos = (t_manejar_block_consola *) args;

	sem_wait(&teclado_consola);
	log_debug(logger, "[registro: %s, respuesta_teclado: %d]", str_registro(datos->reg), respuesta_teclado);
	datos->pcb->registros[datos->reg] = respuesta_teclado;
	
	pasarAReady(datos->pcb);
	free(args);
}

void manejar_op_pantallla(void* args) {
	t_manejar_block_consola* datos = (t_manejar_block_consola *) args;

	sem_wait(&pantalla_consola);
	log_debug(logger, "registro: %s impreso por Proceso: %d]", str_registro(datos->reg), datos->pcb->id);
	
	pasarAReady(datos->pcb);
	free(args);
}

void manejar_op_consola(dispositivos dispo, PCB* pcb, reg_cpu reg) {
	pthread_t hilo;

    t_manejar_block_consola* args = malloc(sizeof(t_manejar_block_consola));
    args->pcb = pcb;
    args->reg = reg;

	switch (dispo) {
	case TECLADO:
		pthread_create(&hilo, NULL, (void*) manejar_op_teclado, (void*) args);
		break;
	case PANTALLA:
		pthread_create(&hilo, NULL, (void*) manejar_op_pantallla, (void*) args);
		break;
	default:
		break;
	}

    pthread_detach(hilo);
}

void manejar_comunicacion(void* void_args) {
	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char* server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case ELEMENTOS_CONSOLA:
			PCB *pcb = crear_pcb(cliente_socket);
			pasarANew(pcb);
			break;
	
		case FIN_POR_EXIT: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			log_pcb(pcb);
			pasarAExit(pcb);
			break;
		}

		case OP_DISCO: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			manejar_suspension_por(DISCO, pcb, cliente_socket);
			break;
		}

		case OP_IMPRESORA: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			manejar_suspension_por(IMPRESORA, pcb, cliente_socket);
			break;
		}

		case OP_PANTALLA: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			uint32_t reg = recibir_registro(cliente_socket);
			
			pasarABlock(pcb, PANTALLA);

			enviar_codop(pcb->socket_consola, OP_PANTALLA);
			enviar_registro(pcb->socket_consola, pcb->registros[reg]);
			
			manejar_op_consola(PANTALLA, pcb, reg);
			break;
		}

		case OP_TECLADO: {
			PCB* pcb = recibir_pcb_de_cpu(cliente_socket);
			reg_cpu reg = recibir_registro(cliente_socket);
			
			pasarABlock(pcb, TECLADO);
			
			log_debug(logger, "Enviando codigo de operacion: OP_TECLADO");
			enviar_codop(pcb->socket_consola, OP_TECLADO);
			
			manejar_op_consola(TECLADO, pcb, reg);
			break;
		}

		case RESPUESTA_PANTALLA: {
			sem_post(&pantalla_consola);
			break;
		}

		case RESPUESTA_TECLADO: {
			respuesta_teclado = recibir_respuesta_consola(cliente_socket);
			sem_post(&teclado_consola);
			break;
		}

		case DEBUG:
			log_debug(logger, "Estoy debuggeando!");
			break;

		case DESCONEXION_CONTROLADA:
			log_info(logger, "El cliente se desconecto de manera esperada");
			close(cliente_socket);
			return;

		case -1:
			log_error(logger, "El cliente se desconecto. Terminando servidor");
			return;

		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	log_warning(logger, "El cliente se desconecto de %s server", server_name);
	return;
}

void suspender_proceso(void* void_args ) {
	t_manejar_block* args = (t_manejar_block*) void_args;
	int tiempo = args->tiempo_de_suspension;
	PCB* pcb = args->pcb;
	free(args);

	sleep(tiempo);

	pasarAReady(pcb);
}

void ejecutar_suspension_en_hilo(PCB* pcb, int tiempo) {
    pthread_t hilo;

    t_manejar_block* args = malloc(sizeof(t_manejar_block));
    args->pcb = pcb;
    args->tiempo_de_suspension = tiempo;

    pthread_create(&hilo, NULL, (void*) suspender_proceso, (void*) args);
    pthread_detach(hilo);
}

void manejar_suspension_por(dispositivos dispo, PCB* pcb, int cliente_socket) {
	int unidades_de_trabajo = recibir_operacion(cliente_socket);
	int tiempo_por_unidad = obtener_tiempo_io(dispo, config->tiempos_io);
	int tiempo_de_suspension = tiempo_por_unidad * unidades_de_trabajo / 1000;
			
	pasarABlock(pcb, dispo);

	log_trace(logger, "SLEEP DE %d", tiempo_de_suspension);
	ejecutar_suspension_en_hilo(pcb, tiempo_de_suspension);
}

int server_escuchar(char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_manejar_conexion_args* args = malloc(sizeof(t_manejar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) manejar_comunicacion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }

    return 0;
}

int conectar_con(char *servername, char *ip, char *puerto) {
	log_info(logger, "Iniciando conexion con %s - Puerto: %s - IP: %s", ip, puerto, servername);

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
	t_list *lista_segm = deserializar_lista_segm(seg);

	PCB *pcb = nuevoPcb(proccess_counter, cliente_socket, lista_ins, lista_segm);
	
	proccess_counter++;

	return pcb;
}
