#include "../include/comunicacion.h"

extern t_log* logger;

void manejar_comunicacion(int cliente_socket) {
		 // Mientras la conexion este abierta
	    while (cliente_socket != -1) {
			int cod_op = recibir_operacion(cliente_socket);

			switch (cod_op) {
				break;
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
}

int server_escuchar(char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        // Se le pasa el file descriptor del socket del cliente pero se podria hacer un struct y enviar mas
        // argumentos de ser necesario.
        pthread_create(&hilo, NULL, (void*) manejar_comunicacion, (void*) cliente_socket);
        pthread_detach(hilo);
        return 1;
    }

    return 0;
}
