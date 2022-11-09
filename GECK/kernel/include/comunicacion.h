/*
 * comunicacion.h
 *
 *  Created on: Sep 29, 2022
 *      Author: ubuntu
 */

#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include "../include/planificadorLargoPlazo.h"
#include "../include/planificadorCortoPlazo.h"

#include <shared.h>
#include <servidor.h>
#include <paquetes.h>
#include <logs.h>

#include <commons/collections/list.h>

#include <semaphore.h>

void manejar_comunicacion(void* void_args);
int server_escuchar(char* server_name, int server_socket);
int conectar_con(char *servername, char *ip, char *puerto);

void suspender_proceso(void* void_args);
void ejecutar_suspension_en_hilo(PCB* pcb, int tiempo);
void manejar_suspension_por(dispositivos dispo, PCB* pcb, int cliente_socket);

PCB* crear_pcb(int cliente_socket);

#endif /* COMUNICACION_H_ */
