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

// valor de case para enviar antes de cerrar un socket, ya implicito en liberar_conexion()
#define DESCONEXION_CONTROLADA 100 

void liberar_conexion(int);

char* leer_archivo_completo(char*);
t_config* abrir_configuracion(char*);

void enviar_codop(int socket_conexion, int cod_op);
void enviar_pid(int socket_conexion, int pid);
void enviar_registro(int socket_conexion, uint32_t reg);
void enviar_valor(int socket_conexion, uint32_t reg);
void enviar_valor(int socket_conexion, uint32_t reg);

void mostrar_mensaje_para_finalizar(char* mensaje);

typedef enum {
	FIFO,
	RR,
	FEEDBACK
} t_algoritmo_planificacion;

#endif /* SHARED_H_ */
