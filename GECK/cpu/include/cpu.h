#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>
#include <logs.h>
#include <semaphore.h>

#include "comunicacion.h"
#include "configuracion.h"

#define DISPATCH_SERVER_NAME "CPU_SERVER_DISPATCH"
#define INTERRUPT_SERVER_NAME "CPU_SERVER_INTERRUPT"

void iniciar_cpu();
int crear_servidor(char* ip, char* puerto, char* server_name);
int conectar_con(char *servername, char *ip, char *puerto);

void hilo_memoria();

void pedir_marco_memoria(PCB* pcb, int direccion);
void leer_de_memoria(int marco);
void escribir_en_memoria(int marco, int valor);

#endif /* CPU_H_ */
