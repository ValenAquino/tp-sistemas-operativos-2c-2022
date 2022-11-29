#include "../include/reemplazos.h"

extern t_log *logger_debug;

extern t_configuracion_memoria *config;

extern t_list *espacios_en_memoria;

extern pthread_mutex_t memoria_principal_mutex;

extern void *memoria_principal;

extern int puntero_clock;

void manejar_reemplazo_en_memoria(pagina_t* pagina) {

	switch(get_algoritmo_reemplazo(config->algoritmo_reemplazo)) {
	case REEMPLAZO_CLOCK:
		ejecutar_reemplazo_clock(pagina);
		break;
	case REEMPLAZO_CLOCK_M:
		ejecutar_reemplazo_clock_modificado(pagina);
		break;
	}

}

void ejecutar_reemplazo_clock(pagina_t* pagina) {

	espacio_memoria_t* espacio_encontrado = list_get(espacios_en_memoria, puntero_clock);

	while(espacio_encontrado->pagina->bit_u == 1) {
		espacio_encontrado->pagina->bit_u = 0;
		aumentar_puntero_clock();
		espacio_encontrado = list_get(espacios_en_memoria, puntero_clock);
	}


}

void aumentar_puntero_clock() {
	int frames_disponibles = config->tam_memoria / config->tam_pagina;
	if (puntero_clock == frames_disponibles - 1) {
		puntero_clock = 0;
		return;
	}
	puntero_clock++;
}

void ejecutar_reemplazo_clock_modificado(pagina_t* pagina) {

}

algoritmo_reemplazo_memoria get_algoritmo_reemplazo(char *algoritmo) {
	if (!strcmp(algoritmo, "CLOCK")) {
		return REEMPLAZO_CLOCK;
	} else if (!strcmp(algoritmo, "CLOCK-M")) {
		return REEMPLAZO_CLOCK_M;
	}
	return -1;
}
