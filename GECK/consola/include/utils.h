/*
 * utils.h
 *
 *  Created on: Sep 15, 2022
 *      Author: ubuntu
 */

#ifndef CONSOLA_UTILS_H_
#define CONSOLA_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>

int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);

#endif /* CONSOLA_UTILS_H_ */
