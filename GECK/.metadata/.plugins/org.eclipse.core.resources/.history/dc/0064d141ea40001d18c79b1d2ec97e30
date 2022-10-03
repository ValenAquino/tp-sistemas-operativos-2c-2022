#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "shared.h"

typedef enum {
	// Agregar los tipos de operaciones para todo el sistema aca:
	ELEMENTOS_CONSOLA,
    DEBUG
} op_code;

//////////// MENSAJES /////////////
bool send_debug(int fd);

// ELEMENTOS_CONSOLA
bool send_elementos_consola(int, t_list*);
bool recv_elementos_consola(int, t_list**);

#endif
