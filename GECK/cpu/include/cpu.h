#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>
#include <logs.h>

#include "../include/comunicacion.h"

#include "comunicacion.h"
#include "configuracion.h"

#define DISPATCH_SERVER_NAME "CPU_SERVER_DISPATCH"
#define INTERRUPT_SERVER_NAME "CPU_SERVER_INTERRUPT"

void iniciar_cpu();
int crear_servidor(char* ip, char* puerto, char* server_name);

#endif /* CPU_H_ */
