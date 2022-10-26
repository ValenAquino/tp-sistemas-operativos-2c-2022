#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <protocolo.h>
#include <servidor.h>
#include <shared.h>
#include <pcb.h>
#include <paquetes.h>

void manejar_comunicacion(void* void_args);
int server_escuchar(char*, int);
int server_escuchar(char* server_name, int server_socket);

#endif /* COMUNICACION_H_ */
