#include "../include/comunicacion.h"

extern t_log* logger_debug;

void manejar_comunicacion(void* void_args) {
		t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
		int cliente_socket = args->fd;
		char* server_name = args->server_name;
		free(args);

		 // Mientras la conexion este abierta
	    while (cliente_socket != -1) {
			int cod_op = recibir_operacion(cliente_socket);

			switch (cod_op) {
			case DEBUG:
				log_debug(logger_debug, "Estoy debuggeando!");
				break;

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

int server_escuchar(char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger_debug, server_name, server_socket);

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
