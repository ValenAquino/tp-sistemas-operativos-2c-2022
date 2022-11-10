#include "../include/configuracion.h"

extern t_log* logger;

t_configuracion_memoria* procesar_config(t_config* config) {
	char *ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char *puerto_escucha =  config_get_string_value(config, "PUERTO_ESCUCHA");
	int  tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
	int  tam_pagina = config_get_int_value(config, "TAM_PAGINA");
	int  entradas_por_tabla = config_get_int_value(config, "ENTRADAS_POR_TABLA");
	int  retardo_memoria = config_get_int_value(config, "RETARDO_MEMORIA");
	char *algoritmo_reemplazo =  config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	int  marcos_por_proceso = config_get_int_value(config, "MARCOS_POR_PROCESO");
	char *path_swap =  config_get_string_value(config, "PATH_SWAP");
	int  tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");

	t_configuracion_memoria *datos = malloc(sizeof(t_configuracion_memoria));
	datos->ip_memoria = malloc(strlen(ip_memoria) + 1);
	strcpy(datos->ip_memoria, ip_memoria);

	datos->puerto_escucha = malloc(strlen(puerto_escucha) + 1);
	strcpy(datos->puerto_escucha, puerto_escucha);

	datos->tam_memoria = (intptr_t) malloc(sizeof(int));
	datos->tam_memoria = tam_memoria;

	datos->tam_pagina = (intptr_t) malloc(sizeof(int));
	datos->tam_pagina = tam_pagina;

	datos->entradas_por_tabla = (intptr_t) malloc(sizeof(int));
	datos->entradas_por_tabla = entradas_por_tabla;

	datos->retardo_memoria = (intptr_t) malloc(sizeof(int));
	datos->retardo_memoria = retardo_memoria;

	datos->algoritmo_reemplazo = malloc(strlen(algoritmo_reemplazo) + 1);
	strcpy(datos->algoritmo_reemplazo, algoritmo_reemplazo);

	datos->marcos_por_proceso = (intptr_t) malloc(sizeof(int));
	datos->marcos_por_proceso = marcos_por_proceso;

	datos->path_swap = malloc(strlen(path_swap) + 1);
	strcpy(datos->path_swap, path_swap);

	datos->tamanio_swap = (intptr_t) malloc(sizeof(int));
	datos->tamanio_swap = tamanio_swap;

	config_destroy(config); // libero la memoria del config
	return datos;
}

void test_read_config(t_configuracion_memoria* config) {
	log_debug(logger,"Leyendo de config: \n"
			"IP_MEMORIA: %s \n"
			"PUERTO_ESCUCHA: %s \n"
			"TAM_MEMORIA: %d \n"
			"TAM_PAGINA: %d \n"
			"ENTRADAS_POR_TABLA: %d \n"
			"RETARDO_MEMORIA: %d \n"
			"ALGORITMO_REEMPLAZO: %s \n"
			"MARCOS_POR_PROCESO: %d \n"
			"PATH_SWAP: %s \n"
			"TAMANIO_SWAP: %d \n",
			config->ip_memoria,
			config->puerto_escucha,
			config->tam_memoria,
			config->tam_pagina,
			config->entradas_por_tabla,
			config->retardo_memoria,
			config->algoritmo_reemplazo,
			config->marcos_por_proceso,
			config->path_swap,
			config->tamanio_swap);
}
