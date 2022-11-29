#include "../include/memoria_principal.h"

extern t_log *logger_debug;

extern t_configuracion_memoria *config;

extern t_list *espacios_en_memoria;

extern pthread_mutex_t memoria_principal_mutex;

extern void *memoria_principal;

extern int puntero_clock;

espacio_memoria_t* get_espacio_libre_en_memoria_principal() {
	bool esta_libre(void *arg) {
		espacio_memoria_t *espacio_en_mp = arg;
		return espacio_en_mp->pagina == NULL;
	}

	return list_find(espacios_libres_en_swap, (void*) esta_libre);
}

void llenar_espacios_libres_en_memoria_principal() {
	int frames_disponibles = config->tam_memoria / config->tam_pagina;

	for (int i = 0; i < frames_disponibles; i++) {
		espacio_memoria_t *espacio_en_mp = (espacio_memoria_t*) malloc(
				sizeof(espacio_memoria_t));

		espacio_en_mp->pos_memoria = i * config->tam_pagina;
		espacio_en_mp->pagina = NULL;

		list_add(espacios_en_memoria, espacio_en_mp);
	}

	log_debug(logger_debug,
			"La lista para manejar espacio libre en memoria tiene %d elementos",
			list_size(espacios_en_memoria));
}

uint32_t leer_de_memoria_principal(int pid, pagina_t *pagina,
		int offset_dentro_de_frame) {
	uint32_t valor_leido_de_memoria;
	int frame = pagina->frame;
	int offset_memoria = frame * config->tam_pagina;
	if (offset_memoria % config->tam_pagina != 0) {
		log_error(logger_debug, "Offset invalido tratando de leer Memoria");
		exit(EXIT_FAILURE);
	}

	log_info(logger, "PID: <%d> - Acción: <LEER> - Dirección física: <%d%d>",
			pid, frame, offset_dentro_de_frame);
	pthread_mutex_lock(&memoria_principal_mutex);

	memcpy(&valor_leido_de_memoria,
			memoria_principal + offset_memoria + offset_dentro_de_frame,
			sizeof(uint32_t));

	pthread_mutex_unlock(&memoria_principal_mutex);

	pagina->bit_u = 1;

	log_debug(logger_debug,
			"Se leyo de memoria el frame: %d, y offset dentro de frame: %d",
			frame, offset_dentro_de_frame);

	return valor_leido_de_memoria;
}

void escribir_en_memoria_principal(int pid, pagina_t *pagina,
		int offset_dentro_de_frame, uint32_t valor_a_guardar_en_mp) {
	int frame = pagina->frame;

	int offset_memoria = frame * config->tam_pagina;
	if (offset_memoria % config->tam_pagina != 0) {
		log_error(logger_debug, "Offset invalido tratando de escribir Memoria");
		exit(EXIT_FAILURE);
	}

	log_info(logger,
			"PID: <%d> - Acción: <ESCRIBIR> - Dirección física: <%d%d>", pid,
			frame, offset_dentro_de_frame);
	pthread_mutex_lock(&memoria_principal_mutex);
	memcpy(memoria_principal + offset_memoria + offset_dentro_de_frame,
			valor_a_guardar_en_mp, sizeof(uint32_t));

	pthread_mutex_unlock(&memoria_principal_mutex);

	pagina->bit_u = 1;
	pagina->bit_m = 1;

	log_debug(logger_debug,
			"Se escribio en memoria el valor %d en el frame: %d, y offset dentro de frame: %d",
			valor_a_guardar_en_mp, frame, offset_dentro_de_frame);

}

void cargar_pagina_en_memoria_principal(int pid, pagina_t *pagina, uint32_t valor_leido_de_swap) {
	espacio_memoria_t *espacio_encontrado = get_espacio_libre_en_memoria_principal();

	if (espacio_encontrado == NULL) {
		log_debug(logger_debug, "No se encontro espacio libre en memoria");
		manejar_reemplazo_en_memoria(pagina);
		return;
	}

	pagina->frame = espacio_encontrado->pos_memoria / config->tam_pagina;
	pagina->bit_p = 1;
	pagina->bit_u = 1;

	espacio_encontrado->pagina = pagina;
	aumentar_puntero_clock();

	escribir_en_memoria_principal(pid, pagina, 0, valor_leido_de_swap);
}

