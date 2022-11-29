#ifndef MEMORIA_PRINCIPAL_H_
#define MEMORIA_PRINCIPAL_H_

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
#include <memoria_shared.h>

#include "configuracion.h"

espacio_memoria_t* get_espacio_libre_en_memoria_principal();
void llenar_espacios_libres_en_memoria_principal();

uint32_t leer_de_memoria_principal(int pid, pagina_t *pagina,
		int offset_dentro_de_frame);
void escribir_en_memoria_principal(int pid, pagina_t *pagina,
		int offset_dentro_de_frame, uint32_t valor_a_guardar_en_mp);

void cargar_pagina_en_memoria_principal(int pid, pagina_t *pagina, uint32_t valor_leido_de_swap);

#endif /* MEMORIA_PRINCIPAL_H_ */
