#include "../include/reemplazos.h"

extern t_log *logger_debug;

extern t_configuracion_memoria *config;

extern t_list *espacios_en_memoria;

extern int puntero_clock;

void manejar_reemplazo_en_memoria(pagina_t *pagina, void *data_leida, int nro_seg, int nro_pag) {

	switch (get_algoritmo_reemplazo(config->algoritmo_reemplazo)) {
	case REEMPLAZO_CLOCK:
		ejecutar_reemplazo_clock(pagina, data_leida, nro_seg, nro_pag);
		break;
	case REEMPLAZO_CLOCK_M:
		ejecutar_reemplazo_clock_mejorado(pagina, data_leida, nro_seg, nro_pag);
		break;
	}

}

void ejecutar_reemplazo_clock(pagina_t *pagina, void *data_leida, int nro_seg, int nro_pag) {
	espacio_memoria_t *espacio_victima = encontrar_espacio_victima_clock(
			pagina->pid);

	if (espacio_victima->pagina->bit_m == 1) {
		espacio_victima->pagina->bit_m = 0;
		void *data_pagina_victima = leer_pagina_entera_de_memoria_principal(
				espacio_victima->pagina);
		cargar_en_swap_pagina_entera(espacio_victima->pagina,
				data_pagina_victima, nro_seg, nro_pag, 1);
	}

	reemplazar_pagina_en_espacio_victima(pagina, espacio_victima);
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

int tengo_que_buscar_otra_pagina_clock(espacio_memoria_t *espacio_encontrado,
		int pid) {
	if (espacio_encontrado->pagina == NULL) {
		return 1;
	}

	if (espacio_encontrado->pagina->bit_u == 1
			|| espacio_encontrado->pagina->pid != pid) {
		return 1;
	}

	return 0;
}

void ejecutar_reemplazo_clock_mejorado(pagina_t *pagina, void *data_leida, int nro_seg, int nro_pag) {
	espacio_memoria_t *espacio_victima =
			encontrar_espacio_victima_clock_mejorado(pagina->pid);

	if (espacio_victima->pagina->bit_m == 1) {
		espacio_victima->pagina->bit_m = 0;
		void *data_pagina_victima = leer_pagina_entera_de_memoria_principal(
				espacio_victima->pagina);
		cargar_en_swap_pagina_entera(espacio_victima->pagina,
				data_pagina_victima, nro_seg, nro_pag, 1);
	}

	reemplazar_pagina_en_espacio_victima(pagina, espacio_victima);
	escribir_pagina_entera_en_memoria_principal(pagina, data_leida);
}

espacio_memoria_t* encontrar_espacio_victima_clock_mejorado(int pid) {
	log_debug(logger_debug,
			"CLOCK MEJORADO - Buscando victima para el proceso <%d>. espacios_en_memoria: %d / Puntero clock: %d",
			pid, list_size(espacios_en_memoria), puntero_clock);
	espacio_memoria_t *espacio_encontrado = list_get(espacios_en_memoria,
			puntero_clock);

	do {
		espacio_encontrado = buscar_cero_cero_clock_m(pid);
		if (espacio_encontrado != NULL)
			break;
		espacio_encontrado = buscar_modificado_clock_m(pid);
	} while (espacio_encontrado == NULL);

	log_debug(logger_debug,
			"Se encontro espacio de memoria victima: %d para el PID: %d",
			espacio_encontrado->pos_memoria, pid);

	return espacio_encontrado;
}

espacio_memoria_t* buscar_modificado_clock_m(int pid) {
	espacio_memoria_t *espacio_encontrado = list_get(espacios_en_memoria,
			puntero_clock);

	int valor_inicial_p_clock = puntero_clock;
	while (condicion_modificado_clock_m(espacio_encontrado, pid)) {
		if (espacio_encontrado->pagina != NULL) {
			if (espacio_encontrado->pagina->pid == pid) {
				espacio_encontrado->pagina->bit_u = 0;
			}
		}

		aumentar_puntero_clock();
		if (valor_inicial_p_clock == puntero_clock) {
			log_debug(logger_debug,
					"PID: <%d> NO Se encontro espacio de memoria victima buscando bit_u == 0 && bit_m == 1",
					pid);
			return NULL;
		}
		espacio_encontrado = list_get(espacios_en_memoria, puntero_clock);
	}

	log_debug(logger_debug,
			"PID: <%d> Se encontro espacio de memoria victima: %d buscando bit_u == 0 && bit_m == 1",
			pid, espacio_encontrado->pos_memoria);

	return espacio_encontrado;
}

espacio_memoria_t* buscar_cero_cero_clock_m(int pid) {
	espacio_memoria_t *espacio_encontrado = list_get(espacios_en_memoria,
			puntero_clock);
	int valor_inicial_p_clock = puntero_clock;

	while (condicion_cero_cero_clock_m(espacio_encontrado, pid)) {
		aumentar_puntero_clock();
		if (valor_inicial_p_clock == puntero_clock) {
			log_debug(logger_debug,
					"PID: <%d> NO Se encontro espacio de memoria victima buscando bit_u == 0 && bit_m == 0",
					pid);
			return NULL;
		}
		espacio_encontrado = list_get(espacios_en_memoria, puntero_clock);
	}
	log_debug(logger_debug,
			"PID: <%d> Se encontro espacio de memoria victima: %d buscando bit_u == 0 && bit_m == 0",
			pid, espacio_encontrado->pos_memoria);
	return espacio_encontrado;
}

int condicion_cero_cero_clock_m(espacio_memoria_t *espacio_encontrado, int pid) {
	if (espacio_encontrado->pagina == NULL) {
		return 1;
	}

	if (espacio_encontrado->pagina->bit_u != 0
			|| espacio_encontrado->pagina->bit_m != 0
			|| espacio_encontrado->pagina->pid != pid) {
		return 1;
	}

	return 0;
}

int condicion_modificado_clock_m(espacio_memoria_t *espacio_encontrado, int pid) {
	if (espacio_encontrado->pagina == NULL) {
		return 1;
	}

	if (espacio_encontrado->pagina->bit_u != 0
			|| espacio_encontrado->pagina->bit_m != 1
			|| espacio_encontrado->pagina->pid != pid) {
		return 1;
	}

	return 0;
}

void reemplazar_pagina_en_espacio_victima(pagina_t *pag_a_guardar,
		espacio_memoria_t *espacio_victima) {

	log_trace(
			logger_debug,
			"pos victima: %ld, pos pag a guardar: %ld",
			espacio_victima->pagina->pos_swap, pag_a_guardar->pos_swap
			);

	espacio_victima->pagina->bit_p = 0;
	espacio_victima->pagina = pag_a_guardar;

	aumentar_puntero_clock();
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
