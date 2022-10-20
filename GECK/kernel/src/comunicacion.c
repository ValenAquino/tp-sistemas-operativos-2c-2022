/*
 * comunicacion.c
 *
 *  Created on: Sep 29, 2022
 *      Author: ubuntu
 */

#include "../include/comunicacion.h"

extern t_log* logger;

int manejar_comunicacion(op_code cod_op, int cliente_fd) {
		switch (cod_op) {
		case ELEMENTOS_CONSOLA:

			t_list *listas = recibir_paquete(cliente_fd);

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
		case DESCONEXION:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return cod_op;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}

		return 1;
}
