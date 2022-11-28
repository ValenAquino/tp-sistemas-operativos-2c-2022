#include "../include/swap.h"

extern FILE* area_swap;

extern t_list *espacios_libres_en_swap;

extern t_configuracion_memoria* config;

extern pthread_mutex_t swap_mutex;

int crear_archivo_swap() {
	char* path = config->path_swap;

    log_info(logger_debug, "Creando SWAP en <<%s>>", path);
    area_swap = fopen(path, "w+");

    if (area_swap == NULL) {
        log_error(logger_debug, "No se pudo crear el area de SWAP. (errno %i)", errno);
        return EXIT_FAILURE;
    }

    truncate(path, config->tamanio_swap);

    if (errno != 0) log_error(logger_debug, "Error en truncate: errno %i", errno);

    return EXIT_SUCCESS;
}

void borrar_de_swap(pagina_t* pagina) {
	cargar_en_swap(pagina, 0, 0);

	bool mismo_offset(void* arg) {
		espacio_swap_t* espacio_swap = arg;
		return espacio_swap->pos_swap == pagina->pos_swap;
	}

	espacio_swap_t* espacio_encontrado = list_find(espacios_libres_en_swap, mismo_offset);
	espacio_encontrado->libre = 1;
}

uint32_t leer_de_swap(pagina_t* pagina, int pid, int nro_seg, int nro_pag) {
	uint32_t valor_leido_de_swap;
	long offset = pagina->pos_swap;
	if (offset % config->tam_pagina != 0) {
		log_error(logger_debug, "Offset invalido tratando de leer SWAP");
		exit(EXIT_FAILURE);
	}

	int cant_bytes_leidos;
	pthread_mutex_lock(&swap_mutex);
	fseek(area_swap, offset, SEEK_SET);
	cant_bytes_leidos = fread(&valor_leido_de_swap, sizeof(uint32_t), 1, area_swap);
	pthread_mutex_unlock(&swap_mutex);
	log_debug(logger_debug, "Se leyeron %d bytes en SWAP", cant_bytes_leidos);

	ejecutar_retardo_swap();

	return valor_leido_de_swap;
}

void cargar_en_swap(pagina_t* pagina, uint32_t valor_a_guardar_en_swap, int ejecutar_retardo) {
	long offset = get_espacio_libre_en_swap();
	if (offset == -1) {
		log_error(logger_debug, "Algo salio mal buscando el proximo espacio disponible en SWAP");
		exit(EXIT_FAILURE);
	}

	int cant_bytes_escritos;
	pthread_mutex_lock(&swap_mutex);
	fseek(area_swap, offset, SEEK_SET);
	cant_bytes_escritos = fwrite(&valor_a_guardar_en_swap, sizeof(uint32_t), 1, area_swap);
	pthread_mutex_unlock(&swap_mutex);
	log_debug(logger_debug, "Se escribieron %d byte/s en SWAP", cant_bytes_escritos);

	log_debug(logger_debug, "Estoy guardando en el offset %ld", offset);
	pagina->pos_swap = offset;

	if (ejecutar_retardo)
		ejecutar_retardo_swap();
}

long get_espacio_libre_en_swap() {
	espacio_swap_t *espacio_encontrado;
	long posicion_libre_swap = -1;

	bool esta_libre(void* arg) {
		espacio_swap_t* espacio_swap = arg;
		return espacio_swap->libre == 1;
	}

	espacio_encontrado = list_find(espacios_libres_en_swap, (void*) esta_libre);
	posicion_libre_swap = espacio_encontrado->pos_swap;

	espacio_encontrado->libre = 0;

	return posicion_libre_swap;
}

void llenar_espacios_libres_swap() {
	int entradas_swap = config->tamanio_swap / config->tam_pagina;

	for (int i = 0; i < entradas_swap; i++) {
		espacio_swap_t* espacio_en_swap = (espacio_swap_t*) malloc(sizeof(espacio_swap_t));

		espacio_en_swap->pos_swap = i * config->tam_pagina;
		espacio_en_swap->libre = 1;

		list_add(espacios_libres_en_swap, espacio_en_swap);
	}

	log_debug(logger_debug, "La lista para manejar espacio libre en swap tiene %d elementos", list_size(espacios_libres_en_swap));
}

void ejecutar_retardo_swap() {
	log_debug(logger_debug, "Ejecutando retardo de SWAP de %d milisegundos", config->retardo_swap);
	usleep(config->retardo_swap * 1000);
}


