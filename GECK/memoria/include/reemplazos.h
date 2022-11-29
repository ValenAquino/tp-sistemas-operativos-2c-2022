
#ifndef REEMPLAZOS_H_
#define REEMPLAZOS_H_

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

#include "configuracion.h"
#include "memoria_principal.h"
#include "memoria_shared.h"
#include "swap.h"

void manejar_reemplazo_en_memoria(pagina_t* pagina, void* data_leida);

algoritmo_reemplazo_memoria get_algoritmo_reemplazo(char *algoritmo);
void aumentar_puntero_clock();
void ejecutar_reemplazo_clock(pagina_t* pagina, void* data_leida);
void ejecutar_reemplazo_clock_modificado(pagina_t* pagina, void* data_leida);

espacio_memoria_t* encontrar_espacio_victima_clock(int pid);

#endif /* REEMPLAZOS_H_ */
