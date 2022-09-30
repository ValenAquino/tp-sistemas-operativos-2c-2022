#ifndef CLIENTE_H_
#define CLIENTE_H_

#include "shared.h"

int crear_conexion(char*, char*);
void enviar_codop(int socket_conexion, int cod_op);

#endif /* CLIENTE_H_ */
