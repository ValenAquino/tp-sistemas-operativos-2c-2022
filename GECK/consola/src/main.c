/*
 * main.c
 *
 *  Created on: Sep 15, 2022
 *      Author: ubuntu
 */

#include "../include/main.h"

t_log* logger;
t_config* config;
int conexion_kernel;

int main(int argc, char** argv) {
	// TODO: tener en cuenta el logger para mas de una consola.
	logger = log_create("Consola.log", "logger", true, LOG_LEVEL_INFO);
	// TODO: Cambiar a 3 cuando recibamos el archivo de pseudocodigo
	if (argc < 2) {
		log_error(logger, "Se necesitan mas argumentos para inicializar correctamente la consola");
		return EXIT_FAILURE;
	}

	config = abrir_configuracion(argv[1]);
	if (config == NULL) {
		log_error(logger, "No se pudo abrir el archivo de configuracion en ese path");
		return EXIT_FAILURE;
	}

	char* ip_kernel = config_get_string_value(config, "IP_KERNEL");
	char* puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	conect_to_kernel(ip_kernel, puerto_kernel);

	return liberar_memoria();
}

int liberar_memoria() {
	liberar_conexion(conexion_kernel);
	log_destroy(logger);
	config_destroy(config);
	return EXIT_SUCCESS;
}

void conect_to_kernel(char* ip, char* puerto) {
	log_info(logger, "Iniciando conexion con el Kernel - Puerto: %s - IP: %s", ip, puerto);
	conexion_kernel = crear_conexion(ip, puerto);
}


