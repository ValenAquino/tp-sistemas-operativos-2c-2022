/*
 * configuracion.h
 *
 *  Created on: Oct 23, 2022
 *      Author: ubuntu
 */

#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <shared.h>

typedef struct t_configuracion_memoria
{
	char *ip_memoria;
	char *puerto_escucha; // Puerto en el cual se escuchará la conexión de módulo.
    int tam_memoria; // Tamaño expresado en bytes del espacio de usuario de la memoria.
    int tam_pagina; // Tamaño de las páginas en bytes.
    int entradas_por_tabla; // Cantidad de entradas de cada tabla de páginas.
    int retardo_memoria; // Tiempo en milisegundos que se deberá esperar para dar una respuesta al CPU.
    char *algoritmo_reemplazo; // Algoritmo de reemplazo de páginas (CLOCK/CLOCK-M).
    int marcos_por_proceso; // Cantidad de marcos permitidos por proceso en asignación fija.
    int retardo_swap; // Tiempo en milisegundos que se deberá esperar para cada operación del SWAP (leer/escribir)
    char *path_swap; // Carpeta donde se van a encontrar los archivos de SWAP de cada proceso.
    int tamanio_swap; //Tamaño del archivo de SWAP.
} t_configuracion_memoria;

t_configuracion_memoria* procesar_config(t_config* config);
void test_read_config(t_configuracion_memoria*);

#endif /* CONFIGURACION_H_ */
