#ifndef SHARED_H_
#define SHARED_H_

#define MAX_CON 10 // maxima cantidad de conexiones entrantes

//Uso general
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <inttypes.h>
#include <stdint.h>

//Sockets
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

//Hilos
#include <pthread.h>

//Commons
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

void liberar_conexion(int);

bool config_has_all_properties(t_config*, char**);
char* leer_archivo_completo(char*);
t_config* abrir_configuracion(char*);

#endif /* SHARED_H_ */
