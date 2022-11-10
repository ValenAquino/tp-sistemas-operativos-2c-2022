/*
 * comunicacion.h
 *
 *  Created on: Sep 29, 2022
 *      Author: ubuntu
 */

#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include "planificadorLargoPlazo.h"
#include "planificadorCortoPlazo.h"
#include "suspendidos.h"
#include <shared.h>
#include <servidor.h>
#include <paquetes.h>
#include <logs.h>
#include <commons/collections/list.h>
#include <semaphore.h>

void manejar_comunicacion(void* void_args);
int server_escuchar(char* server_name, int server_socket);
int conectar_con(char *servername, char *ip, char *puerto);

PCB* crear_pcb(int cliente_socket);

#endif /* COMUNICACION_H_ */
