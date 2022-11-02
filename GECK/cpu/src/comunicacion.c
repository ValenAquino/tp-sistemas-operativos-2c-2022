#include "../include/comunicacion.h"
extern t_log* logger;

extern int FLAG_INTERRUPT;

void manejar_comunicacion(void* void_args) {
	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	int cliente_socket = args->fd;
	char* server_name = args->server_name;
	free(args);

	// Mientras la conexion este abierta
	while (cliente_socket != -1) {
		log_trace(logger, "Esperando codigo de operacion [%s]", server_name);
		int cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) {
		case DISPATCH_PCB:
			log_trace(logger, "Recibiendo PCB");
			PCB* pcb = recibir_pcb(cliente_socket);
			
			log_trace(logger, "Enviando PCB al ciclo de instruccion\n");
			ciclo_de_instruccion(pcb, cliente_socket);
			log_trace(logger, "Ciclo de instruccion completado\n");
			break;

		case DEBUG:
			log_debug(logger, "Estoy debuggeando!\n");
			break;

		case DESCONEXION_CONTROLADA:
			log_info(logger, "El cliente se desconecto de manera esperada\n");
			close(cliente_socket);
			return;
		
		case INTERRUPT:
			FLAG_INTERRUPT = 1;
			//manejar_interrupcion();
			break;

		case -1:
			log_error(logger, "El cliente se desconecto. Terminando servidor [%s]", server_name);
			// close(cliente_socket);
			// exit(EXIT_FAILURE);
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

