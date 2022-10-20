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

#include "comunicacion.h"

#define SERVERNAME "KERNEL_SERVER"

int iniciar_servidor_kernel(char* ip, char* puerto);

typedef struct t_configuracion_kernel
{
    char *ip_memoria;
    char *ip_kernel;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
    char *puerto_escucha;
    char *algoritmo_planificacion;
    char * grado_max_multiprogramacion;
    char * quantum_rr;
    char **dispositivos_io;
    char **tiempos_io;
} t_configuracion_kernel;

#endif /* KERNEL_MAIN_H_ */
