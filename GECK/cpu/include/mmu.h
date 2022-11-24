#ifndef MMU_H_
#define MMU_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <shared.h>
#include <logs.h>

#include <commons/log.h>

typedef struct {
	int id_tabla_pagina;
	int nro_pag;
	int desplazamiento_pag;
	int nro_seg;
} dir_t; // mover a shared

#endif /* MMU_H_ */
