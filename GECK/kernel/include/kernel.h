/*
 * main.h
 *
 *  Created on: Sep 15, 2022
 *      Author: ubuntu
 */

#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>

typedef enum {
	FIFO,
	RR,
	FEEDBACK
} t_algoritmo_planificacion;

#include "comunicacion.h"
#include "configuracion.h"

#define SERVERNAME "KERNEL_SERVER"

int iniciar_servidor_kernel(char* ip, char* puerto);
void inicializar_kernel();
void hilo_escucha_dispatch();

#endif /* KERNEL_MAIN_H_ */
