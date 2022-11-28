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
#include <math.h>

#include "comunicacion.h"
#include "configuracion.h"

int obtener_num_marco(dir_t direccion_parcial, int pid);
int leer(int marco);
void escribir(int marco, int valor);

int cargar_en_memoria_principal(pagina_t *pagina, uint32_t valor_leido_de_swap);

t_list* crear_indices_tabla_de_paginas(t_list *tamanio_segmentos, int pid) ;
t_list* crear_paginas();

pagina_t* obtener_pagina(int id_en_tablas_de_ps, int nro_pag);
pagina_t *crear_pagina_vacia();

void ejecutar_retardo_memoria(char* motivo);

#endif /* INTERFAZ_H_ */
