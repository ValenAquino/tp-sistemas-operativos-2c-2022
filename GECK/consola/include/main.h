/*
 * main.h
 *
 *  Created on: Sep 15, 2022
 *      Author: ubuntu
 */

#ifndef CONSOLA_MAIN_H_
#define CONSOLA_MAIN_H_

#include <stdlib.h>
#include <commons/log.h>
#include <utils.h>
#include "utils.h"

int liberar_memoria();
void conect_to_kernel(char* ip, char* puerto);
#endif /* CONSOLA_MAIN_H_ */
