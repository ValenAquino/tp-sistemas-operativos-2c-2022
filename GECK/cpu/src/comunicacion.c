#include "../include/comunicacion.h"
extern t_log* logger;

void manejar_comunicacion(void* void_args) {
	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char* server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case DISPATCH_PCB:
			t_list *listas = recibir_paquete(cliente_socket);

			void* datos = list_get(listas, 0);
			void* inst  = list_get(listas, 1);
			void* segm  = list_get(listas, 2);

			log_trace(logger, "size list: %d", list_size(listas));
			list_destroy(listas);
			
			PCB* pcb = deserializar_pcb(datos, inst, segm);
			log_pcb(pcb);
			
		    pcb->registros[0] = 12;
			pcb->registros[1] = 34;
			pcb->registros[2] = 56;
			pcb->registros[3] = 78;

			enviar_pcb(pcb, cliente_socket, DISPATCH_PCB);

			free(datos);
			free(inst);
			free(segm);
			free(pcb);
			
			break;
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

