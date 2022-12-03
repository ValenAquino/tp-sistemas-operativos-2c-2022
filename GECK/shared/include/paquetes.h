#ifndef SHAREDLOCAL_H_
#define SHAREDLOCAL_H_

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
#include "shared.h"
#include "cliente.h"
#include "protocolo.h"
#include "pcb.h"
#include "memoria_shared.h"

typedef enum {
	SET, ADD, MOV_IN, MOV_OUT, IO, EXIT
} t_ins;

typedef enum {
	AX, BX, CX, DX
} reg_cpu;

typedef enum {
	TECLADO, PANTALLA, DISCO, IMPRESORA
} dispositivos; // TODO: Sacar DISCO, IMPRESORA cuando se modifiquen todos los lugares donde se usa

typedef struct {
		char *nombre;
		int tiempo_retardo;
		sem_t sem_dispositivo;
} ts_dispositivo;

typedef struct {
	t_ins name;
	char *param1;
	char *param2;
} ts_ins_consola;

typedef struct {
	t_ins name;
	int param1;
	int param2;
} ts_ins;

typedef struct {
	int size;
	void *stream;
} ts_buffer;

typedef struct {
	op_code code;
	ts_buffer *buffer;
} ts_paquete;

#define size_caracter_fin_de_cadena 1 // --> '\0'

// Buffer
void crear_buffer(ts_paquete *paquete);
void* recibir_buffer(int*, int);

// Paquetes
void agregar_a_paquete(ts_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(ts_paquete *paquete, int socket_cliente);
void eliminar_paquete(ts_paquete *paquete);
ts_paquete* crear_paquete(op_code);
void* serializar_paquete(ts_paquete *paquete, int bytes);
t_list* recibir_paquete(int);

void enviar_pcb(PCB *pcb, int socket_fd, op_code op_code);
void enviar_solicitud_crear_estructuras_memoria(t_list *tamanios_segmentos, int socket_fd, int pid);
void enviar_indices_tablas_de_paginas(t_list *indices, int socket_fd, int pid);
void enviar_direccion_parcial(dir_t direccion, int cliente_socket, int codop);

// Serializacion
void* serializar_lista_indices(t_list*, int);
void* serializar_lista_ins(t_list*, int);
void* serializar_lista_seg(t_list*, int);
void* serializar_lista_tamanios_seg(t_list *lista, int size, int destruir_lista);
void* serializar_datos_pcb(PCB*, int);
void* serializar_direccion_parcial(dir_t, int);
void* serializar_lista_ins_consola(t_list *lista, int size_stream);

// Deserializacion
t_list* deserializar_lista_inst(void *stream);
t_list* deserializar_lista_ins_consola(void *stream);
t_list* deserializar_lista_segm(void *stream);
t_list* deserializar_lista_tamanios_segm(void *stream);
t_list* deserializar_lista_io(void *stream);
t_list* deserializar_lista_tiempos(void *stream);
t_list* deserializar_lista_indices(void *stream);
PCB* deserializar_pcb(void *data, void *inst, void *segm, void *tamanios_segmentos);
dir_t deserializar_direccion_parcial(void *stream);

// Recepcion
int recibir_operacion(int);
PCB* recibir_pcb(int cliente_socket);
t_list* recibir_solicitud_crear_estructuras_memoria(int cliente_socket);
t_list* recibir_indices_tablas_de_paginas(int cliente_socket);
dir_t recibir_direccion_parcial(int cliente_socket);

#endif /* SHAREDLOCAL_H_ */
