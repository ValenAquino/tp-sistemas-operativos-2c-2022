#ifndef MMU_H_
#define MMU_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <paquetes.h>
#include <shared.h>
#include <servidor.h>
#include <logs.h>
#include <memoria_shared.h>
#include <commons/log.h>
#include <math.h>

#include "tlb.h"

dir_t traducir_direccion(int dir_logica, t_list *tabla_segmentos);

int pedir_marco_memoria(int pid, dir_t dir_parcial, int memoria_fd);
void leer_de_memoria(int marco, int offset, int memoria_fd);
void escribir_en_memoria(int marco, int valor);

// CALCULOS
int get_numero_segmento(int dir_logica);
int get_desplazamiento_segmento(int dir_logica);
int get_numero_pagina(int desplazamiento_segmento);
int get_desplazamiento_pagina(int desplazamiento_segmento);
int hay_seg_fault(int desplazamiento_segmento, int tamanio_segmento);

#endif /* MMU_H_ */
