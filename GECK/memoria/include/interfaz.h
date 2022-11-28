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
#include <logs.h>

#include "comunicacion.h"
#include "configuracion.h"

int obtener_num_marco(dir_t direccion_parcial);
int leer(int marco);
void escribir(int marco, int valor);

t_list *crear_indices_tabla_de_paginas(t_list* tamanio_segmentos);
t_list* crear_paginas();
pagina_t *crear_pagina_vacia();

#endif /* INTERFAZ_H_ */
