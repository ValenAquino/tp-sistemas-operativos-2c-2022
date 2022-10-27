/*
 * memoria.h
 *
 *  Created on: Oct 3, 2022
 *      Author: ubuntu
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>

#include "comunicacion.h"
#include "configuracion.h"

#define SERVERNAME "MEMORIA_SERVER"

int crear_conexion(char* ip, char* puerto);
void iniciar_memoria();

#endif /* MEMORIA_H_ */
