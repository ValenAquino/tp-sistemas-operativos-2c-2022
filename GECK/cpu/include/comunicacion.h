#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <protocolo.h>
#include <servidor.h>
#include <shared.h>
#include <pcb.h>
#include <paquetes.h>
#include <logs.h>

#include "cicloDeInstruccion.h"
#include "configuracion.h"
#include <execinfo.h>


/* Obtain a backtrace and print it to stdout. */
void print_trace();
void inciar_comunicacion_con_memoria();

void manejar_comunicacion_dispatch(void* void_args);
void manejar_comunicacion_interrupt(void* void_args);

//void manejar_comunicacion_memoria(void* void_args);

int server_escuchar_interrupt(char* server_name, int server_interrupt_fd);
int server_escuchar_dispatch(char* server_name, int server_dispatch_fd);
int conectar_con(char *servername, char *ip, char *puerto);

#endif /* COMUNICACION_H_ */
