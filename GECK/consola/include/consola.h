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
#include <protocolo.h>
#include <shared.h>
#include <cliente.h>

int procesar_config(
			char *config_path,
			t_list **lista_segmentos
		);
int conect_to_kernel(char*, char*);
int liberar_memoria(t_log*, int);

#endif /* CONSOLA_H_ */
