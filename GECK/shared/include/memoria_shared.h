#ifndef MEMORIA_SHARED_H_
#define MEMORIA_SHARED_H_

#include "shared.h"

typedef struct {
	int id_tabla_pagina;
	int nro_pag;
	int desplazamiento_pag;
	int nro_seg;
} dir_t;

typedef struct {
	int tamanio_segmento;
	int indice_tablas_paginas;
} segmento_t;

typedef struct {
	int pid;
	int bit_p;
	int bit_u;
	int bit_m;
	int frame;
	long pos_swap;
} pagina_t;

typedef struct {
	int pos_swap;
	int libre;
} espacio_swap_t;

typedef struct {
	pagina_t* pagina;
	int pos_memoria;
} espacio_memoria_t;

typedef enum {
	REEMPLAZO_CLOCK,
	REEMPLAZO_CLOCK_M
} algoritmo_reemplazo_memoria;

typedef enum {
	SEG_FAULT_ERROR = -1,
	PAGE_FAULT_ERROR = -2
} error_memoria;

typedef struct {
	PCB* pcb;
	int nro_segmento_solicitado;
	int indice_tablas_de_paginas;
	int pagina_solicitada;
} t_manejar_page_fault;

#endif /* MEMORIA_SHARED_H_ */
