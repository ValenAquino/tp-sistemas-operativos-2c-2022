/*
 * comunicacion.h
 *
 *  Created on: Oct 3, 2022
 *      Author: ubuntu
 */

#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <protocolo.h>
#include <shared.h>

void manejar_comunicacion(int cliente_socket);
int server_escuchar(char*, int);

#endif /* COMUNICACION_H_ */
