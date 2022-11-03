#include "../include/comunicacion.h"

extern t_log* logger;
int proccess_counter = 0;

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

			//log_list_inst(pcb->instrucciones);
			//log_lista_seg(pcb->tablaSegmentos);
		
			nuevoProceso(pcb);
			
			PCB* pcb_a_ejeutar = get_siguiente_proceso();
			pasarAExec(pcb_a_ejeutar);
			
			break;


		case FIN_POR_EXIT: {
			PCB* pcb = recibir_pcb(cliente_socket);
			log_pcb(pcb);
			pasarAExit(pcb);
			break;
		}

		case OP_IO: {
			PCB* pcb = recibir_pcb(cliente_socket);
			log_pcb(pcb);
			pasarAExec(pcb);
			break;
		}


		case SIGUIENTE_PROCESO:
			PCB *siguiente_pcb = get_siguiente_proceso();
			dispatch_pcb(siguiente_pcb);
			return;

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
