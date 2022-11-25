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
	int num_pagina;
} segmento_t;

typedef struct {
	int bit_p;
	int bit_u;
	int bit_m;
	int frame;
} pagina_t;

typedef enum {
	SEG_FAULT_ERROR = -1,
	PAGE_FAULT_ERROR = -2
} error_memoria;

#endif /* MEMORIA_SHARED_H_ */
