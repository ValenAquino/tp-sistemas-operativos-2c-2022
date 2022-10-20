/*
 * comunicacion.c
 *
 *  Created on: Sep 29, 2022
 *      Author: ubuntu
 */

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
				case ELEMENTOS_CONSOLA:

				t_list *listas = recibir_paquete(cliente_socket);

				void* ins = list_get(listas, 0);
				void* seg = list_get(listas, 1);

				list_destroy(listas);

				t_list *lista_ins = deserializar_lista_inst(ins);
				t_list *lista_segm = deserializar_lista_segm(seg);

				log_debug(logger, "instrucciones: ");
				for (int i = 0; i < list_size(lista_ins); i++) {
					ts_ins *ins = list_get(lista_ins, i);
					log_debug(logger, "%d %d %d", ins->name, ins->param1, ins->param2);
				}

				log_debug(logger, "segmentos: ");
				for (int i = 0; i < list_size(lista_segm); i++) {
					int *seg = list_get(lista_segm, i);
					log_debug(logger, "%d", *seg);
				}

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

int conectar_cpu(char* ip, char* puerto) {
	log_info(logger, "Iniciando conexion con la CPU - Puerto: %s - IP: %s", ip, puerto);
	return crear_conexion(ip, puerto);
}

int conectar_memoria(char* ip, char* puerto) {
	log_info(logger, "Iniciando conexion con la Memoria - Puerto: %s - IP: %s", ip, puerto);
	return crear_conexion(ip, puerto);
}

