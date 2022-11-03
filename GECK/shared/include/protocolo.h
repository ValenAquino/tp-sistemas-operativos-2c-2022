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
	OP_IO,
	TIEMPOS_IO
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
