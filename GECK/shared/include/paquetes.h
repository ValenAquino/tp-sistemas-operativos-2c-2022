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

typedef enum {
	SET,
	ADD,
	MOV_IN,
	MOV_OUT,
	IO,
	EXIT
} t_ins;

typedef enum {
	AX,
	BX,
	CX,
	DX
} reg_cpu;

typedef enum {
	DISCO,
	TECLADO,
	PANTALLA
} dispositivos;

typedef struct {
	t_ins name;
	int param1;
	int param2;
} ts_ins;

typedef struct
{
	int size;
	void* stream;
} ts_buffer;

typedef struct {
	op_code code;
	ts_buffer* buffer;
} ts_paquete;

// Envio
ts_paquete* crear_paquete(op_code);
void crear_buffer(ts_paquete* paquete);
void agregar_a_paquete(ts_paquete* paquete, void* valor, int tamanio);
void* serializar_paquete(ts_paquete* paquete, int bytes);
void enviar_paquete(ts_paquete* paquete, int socket_cliente);
void eliminar_paquete(ts_paquete* paquete);
void enviar_pcb(PCB* pcb, int socket_fd, op_code op_code);

// Serializacion
void* serializar_lista_ins(t_list*, int);
void* serializar_lista_seg(t_list*, int);
void* serializar_datos_pcb(PCB*, int);

// Deserializacion
t_list* deserializar_lista_inst(void *stream);
t_list* deserializar_lista_segm(void *stream);
PCB* deserializar_pcb(void* data, void* inst, void* segm);

// Recepcion
int recibir_operacion(int);
void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);

#endif /* SHAREDLOCAL_H_ */
