#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "shared.h"

typedef enum {
	// Agregar los tipos de operaciones para todo el sistema aca:
	DESCONEXION = -1,
	ELEMENTOS_CONSOLA,
    DEBUG,
    DISPATCH_PCB,
	FIN_POR_EXIT,
	SIGUIENTE_PROCESO,
	INTERRUPT,
	OP_PANTALLA,
	OP_DISCO,
	OP_TECLADO,
	OP_IMPRESORA,
	TIEMPOS_IO,
	RESPUESTA_PANTALLA,
	RESPUESTA_TECLADO,
	INTERRUPCION_QUANTUM,
	DESALOJO_QUANTUM,
	PAGE_FAULT_CPU,  // MENSAJE QUE MANDA CPU A KERNEL
	PAGINA_SOLICITADA, // MENSAJE QUE MANDA KERNEL A MEMORIA
	PAGINA_ENCONTRADA // MENSAJE QUE MANDA MEMORIA A KERNEL EN RESPUESTA A PAGINA_SOLICITADA.
} op_code;

typedef struct {
    int fd;
    char* server_name;
} t_manejar_conexion_args;

//////////// MENSAJES /////////////
bool send_debug(int fd);

// ELEMENTOS_CONSOLA
bool send_elementos_consola(int, t_list*);
bool recv_elementos_consola(int, t_list**);

#endif
