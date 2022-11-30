#include "../include/reemplazos.h"

extern t_log *logger_debug;

extern t_configuracion_memoria *config;

extern t_list *espacios_en_memoria;

extern int puntero_clock;

void manejar_reemplazo_en_memoria(pagina_t *pagina, void *data_leida) {

	switch (get_algoritmo_reemplazo(config->algoritmo_reemplazo)) {
	case REEMPLAZO_CLOCK:
		ejecutar_reemplazo_clock(pagina, data_leida);
		break;
	case REEMPLAZO_CLOCK_M:
		ejecutar_reemplazo_clock_modificado(pagina, data_leida);
		break;
	}

}

void ejecutar_reemplazo_clock(pagina_t *pagina, void *data_leida) {
	espacio_memoria_t *espacio_victima = encontrar_espacio_victima_clock(
			pagina->pid);

	if (espacio_victima->pagina->bit_m == 1) {
		espacio_victima->pagina->bit_m = 0;
		void *data_pagina_victima = leer_pagina_entera_de_memoria_principal(
				espacio_victima->pagina);
		cargar_en_swap_pagina_entera(pagina, data_pagina_victima, 1);
	}

	espacio_victima->pagina->bit_p = 0;

	escribir_pagina_entera_en_memoria_principal(pagina, data_leida);
}

espacio_memoria_t* encontrar_espacio_victima_clock(int pid) {
	log_debug(logger_debug,
			"CLOCK - Buscando victima para el proceso <%d>. espacios_en_memoria: %d / Puntero clock: %d",
			pid, list_size(espacios_en_memoria), puntero_clock);
	espacio_memoria_t *espacio_encontrado = list_get(espacios_en_memoria,
			puntero_clock);


	while (tengo_que_buscar_otra_pagina_clock(espacio_encontrado, pid)) {

		if (espacio_encontrado->pagina != NULL) {
			if (espacio_encontrado->pagina->pid == pid) {
				espacio_encontrado->pagina->bit_u = 0;
			}
		}

		aumentar_puntero_clock();
		espacio_encontrado = list_get(espacios_en_memoria, puntero_clock);
	}

	log_debug(logger_debug,
			"Se encontro espacio de memoria victima: %d para el PID: %d",
			espacio_encontrado->pos_memoria, pid);

	return espacio_encontrado;
}

int tengo_que_buscar_otra_pagina_clock(espacio_memoria_t* espacio_encontrado, int pid) {
	if (espacio_encontrado->pagina == NULL) {
		return 1;
	}

	if (espacio_encontrado->pagina->bit_u == 1
			|| espacio_encontrado->pagina->pid != pid) {
		return 1;
	}

	return 0;
}

void ejecutar_reemplazo_clock_modificado(pagina_t *pagina, void *data_leida) {

}

void aumentar_puntero_clock() {
	int frames_disponibles = config->tam_memoria / config->tam_pagina;
	if (puntero_clock == frames_disponibles - 1) {
		puntero_clock = 0;
		log_debug(logger_debug,
				"Se reinicio el puntero clock. Valor actual: %d",
				puntero_clock);
		return;
	}
	puntero_clock++;
	log_debug(logger_debug, "Se aumento el puntero clock. Valor actual: %d",
			puntero_clock);
}

algoritmo_reemplazo_memoria get_algoritmo_reemplazo(char *algoritmo) {
	if (!strcmp(algoritmo, "CLOCK")) {
		return REEMPLAZO_CLOCK;
	} else if (!strcmp(algoritmo, "CLOCK-M")) {
		return REEMPLAZO_CLOCK_M;
	}
	return -1;
}
