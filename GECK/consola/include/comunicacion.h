/*
 * comunicacion.h
 *
 *  Created on: Oct 27, 2022
 *      Author: ubuntu
 */

#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include <shared.h>
#include <protocolo.h>
#include <cliente.h>
#include <servidor.h>
#include <paquetes.h>
#include <logs.h>

#include <commons/collections/list.h>

int manejar_comunicacion(void* void_args);

#endif /* COMUNICACION_H_ */

