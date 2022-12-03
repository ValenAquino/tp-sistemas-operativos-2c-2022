/*
 * interfaz.h
 *
 *  Created on: Nov 11, 2022
 *      Author: ubuntu
 */

#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>
#include <pthread.h>
#include <logs.h>
#include <math.h>

#include "comunicacion.h"
#include "configuracion.h"
#include "memoria_principal.h"

int obtener_num_marco(dir_t direccion_parcial, int pid);

uint32_t leer(int pid, dir_t dir_parcial);
void escribir(int pid, dir_t dir_parcial, int valor);

t_list* crear_indices_tabla_de_paginas(t_list *tamanio_segmentos, int pid);
t_list* crear_paginas(int pid);

pagina_t* obtener_pagina(int id_en_tablas_de_ps, int nro_pag);
pagina_t *crear_pagina_vacia();

void ejecutar_retardo_memoria(char* motivo);

#endif /* INTERFAZ_H_ */
