#include "../include/comunicacion.h"

extern t_log* logger;
//extern t_list* procesosNew; //seria una cola
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
			t_list *listas = recibir_paquete(cliente_socket);

			void* ins = list_get(listas, 0);
			void* seg = list_get(listas, 1);

			t_list *lista_ins = deserializar_lista_inst(ins);
			t_list *lista_segm = deserializar_lista_segm(seg);

			PCB *pcb = nuevoPcb(proccess_counter, lista_ins, lista_segm);
			proccess_counter++;

			log_list_inst(pcb->instrucciones);
			log_lista_seg(pcb->tablaSegmentos);

			nuevoProceso(pcb);
			dispatch_pcb(pcb); // PRUEBA

			break;
		case DISPATCH_PCB: {
			t_list *listas = recibir_paquete(cliente_socket);

			void* datos = list_get(listas, 0);
			void* inst  = list_get(listas, 1);
			void* segm  = list_get(listas, 2);

			log_trace(logger, "size list: %d", list_size(listas));
			list_destroy(listas);
			
			PCB* pcb = deserializar_pcb(datos, inst, segm);

			log_pcb(pcb);
			
			free(datos);
			free(inst);
			free(segm);
			free(pcb);

			break;
		}
		case MANEJAR_EXIT: {
			// TODO: Actualmente cliente_socket es el file descriptor de la CPU
			// Necesitamos para este caso el socket de la consola para avisarle
			// que tiene que terminar el proceso.
			t_list *listas = recibir_paquete(cliente_socket);

			void* datos = list_get(listas, 0);
			void* inst  = list_get(listas, 1);
			void* segm  = list_get(listas, 2);

			log_trace(logger, "size list: %d", list_size(listas));
			list_destroy(listas);

			PCB* pcb = deserializar_pcb(datos, inst, segm);

			pasarAExit(pcb, cliente_socket);

			free(datos);
			free(inst);
			free(segm);
			// Aca no se hace el free del pcb porque en pasarAExit se usa.
			break;
		}
		case DEBUG:
			log_debug(logger, "Estoy debuggeando!");
			break;
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
