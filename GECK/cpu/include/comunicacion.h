#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <protocolo.h>
#include <servidor.h>
#include <shared.h>
#include <pcb.h>
#include <paquetes.h>
#include <logs.h>

#include "cicloDeInstruccion.h"

void manejar_comunicacion_dispatch(void* void_args);
void manejar_comunicacion_interrupt(void* void_args);

int server_escuchar_interrupt(char* server_name, int server_interrupt_fd);
int server_escuchar_dispatch(char* server_name, int server_dispatch_fd);


#endif /* COMUNICACION_H_ */
