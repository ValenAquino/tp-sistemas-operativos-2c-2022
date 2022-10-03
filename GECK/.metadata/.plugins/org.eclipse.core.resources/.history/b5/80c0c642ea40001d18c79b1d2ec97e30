/*
 * comunicacion.c
 *
 *  Created on: Sep 29, 2022
 *      Author: ubuntu
 */

#include "../include/comunicacion.h"

extern t_log* logger;

int manejar_comunicacion(op_code cod_op) {
		switch (cod_op) {
		case ELEMENTOS_CONSOLA:
			// Parsear segmentos
			break;
		case DEBUG:
			log_debug(logger, "Estoy debuggeando!");
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return cod_op;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}

}
