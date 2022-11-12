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

int obtener_num_marco(int direccion);
int leer(int marco);
void escribir(int marco, int valor);

#endif /* INTERFAZ_H_ */
