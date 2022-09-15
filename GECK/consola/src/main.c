/*
 * main.c
 *
 *  Created on: Sep 15, 2022
 *      Author: ubuntu
 */

#include "../include/main.h"

t_log* logger;

int main() {

	int conexion;

	// TODO: tener en cuenta el logger para mas de una consola.
	logger = log_create("Consola.log", "logger", true, LOG_LEVEL_INFO);

	log_info(logger, "Iniciando conexion con el Kernel");

	conexion = crear_conexion("127.0.0.1", "4444");

	liberar_memoria(conexion, logger);

	return 0;
}

void liberar_memoria(int conexion, t_log* logger) {
	liberar_conexion(conexion);
	log_destroy(logger);
}
