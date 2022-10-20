/*
 * comunicacion.h
 *
 *  Created on: Sep 29, 2022
 *      Author: ubuntu
 */

#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <protocolo.h>
#include <cliente.h>
#include <servidor.h>
#include <shared.h>
#include <sharedLocal.h>
#include <commons/collections/list.h>

void manejar_comunicacion(void* void_args);
int server_escuchar(char* server_name, int server_socket);
int conectar_cpu(char* ip, char* puerto);
int conectar_memoria(char* ip, char* puerto);

#endif /* COMUNICACION_H_ */
