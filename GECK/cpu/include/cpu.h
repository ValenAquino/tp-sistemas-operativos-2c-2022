/*
 * cpu.h
 *
 *  Created on: Oct 2, 2022
 *      Author: ubuntu
 */

#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>

#define DISPATCH_SERVER_NAME "CPU_SERVER_DISPATCH"
#define INTERRUPT_SERVER_NAME "CPU_SERVER_INTERRUPT"

int crear_servidor(char* ip, char* puerto, char* server_name);

#endif /* CPU_H_ */
