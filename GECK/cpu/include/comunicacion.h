/*
 * comunicacion.h
 *
 *  Created on: Oct 3, 2022
 *      Author: ubuntu
 */

#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <protocolo.h>
#include <servidor.h>
#include <shared.h>

void manejar_comunicacion(void* void_args);
int server_escuchar(char*, int);

#endif /* COMUNICACION_H_ */
