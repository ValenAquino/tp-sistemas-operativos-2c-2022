/*
 * utils.h
 *
 *  Created on: Sep 15, 2022
 *      Author: ubuntu
 */

#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

extern t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(char*, char*);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);

#endif /* KERNEL_UTILS_H_ */
