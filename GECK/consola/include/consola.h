#ifndef CONSOLA_H_
#define CONSOLA_H_

// basicas
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// sockets
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>

// commons
#include<commons/log.h>
#include<commons/config.h>

// shared
#include <shared.h>
#include <cliente.h>
#include <protocolo.h>

// Consola
#include "pseudocodigo.h"

t_config* procesar_config(char *, t_list**, int*);
t_list* parsear_pseudocod(char*);

int connect_to_kernel(t_config*);
void enviar_proceso(int, t_list*, t_list*);

int liberar_memoria(t_log*, int, t_config*);

#endif /* CONSOLA_H_ */
