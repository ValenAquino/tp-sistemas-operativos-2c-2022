#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>
#include <logs.h>
#include <errno.h>
#include <pthread.h>

#include "comunicacion.h"
#include "configuracion.h"

#define SERVERNAME "MEMORIA_SERVER"

int crear_conexion(char* ip, char* puerto);
void iniciar_memoria();
int cargar_memoria();

#endif /* MEMORIA_H_ */
