
#ifndef REEMPLAZOS_H_
#define REEMPLAZOS_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>
#include <logs.h>
#include <errno.h>
#include <pthread.h>

#include "configuracion.h"
#include "memoria_principal.h"
#include "memoria_shared.h"
#include "swap.h"

void manejar_reemplazo_en_memoria(pagina_t* pagina, void* data_leida, int nro_seg, int nro_pag);
algoritmo_reemplazo_memoria get_algoritmo_reemplazo(char *algoritmo);
void reemplazar_pagina_en_espacio_victima(pagina_t *pag_a_guardar,
		espacio_memoria_t *espacio_victima);

// CLOCK
void aumentar_puntero_clock();
void ejecutar_reemplazo_clock(pagina_t* pagina, void* data_leida, int nro_seg, int nro_pag);
espacio_memoria_t* encontrar_espacio_victima_clock(int pid);
int tengo_que_buscar_otra_pagina_clock(espacio_memoria_t* espacio_encontrado, int pid);

//CLOCK-M
void ejecutar_reemplazo_clock_mejorado(pagina_t *pagina, void *data_leida, int nro_seg, int nro_pag);
espacio_memoria_t* encontrar_espacio_victima_clock_mejorado(int pid);
espacio_memoria_t* buscar_modificado_clock_m(int pid);
espacio_memoria_t* buscar_cero_cero_clock_m(int pid);
int condicion_cero_cero_clock_m(espacio_memoria_t *espacio_encontrado, int pid);
int condicion_modificado_clock_m(espacio_memoria_t *espacio_encontrado, int pid);


#endif /* REEMPLAZOS_H_ */
