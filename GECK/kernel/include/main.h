/*
 * main.h
 *
 *  Created on: Sep 15, 2022
 *      Author: ubuntu
 */

#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>

#include "comunicacion.h"

#define SERVERNAME "KERNEL_SERVER"

int iniciar_servidor_kernel(char* ip, char* puerto);

#endif /* KERNEL_MAIN_H_ */
