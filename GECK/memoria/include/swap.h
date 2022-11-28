#ifndef SWAP_H_
#define SWAP_H_

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

#include <memoria_shared.h>
#include "configuracion.h"

int crear_archivo_swap();
void borrar_de_swap(pagina_t* pagina);
uint32_t leer_de_swap(pagina_t* pagina);
void cargar_en_swap(pagina_t* pagina, uint32_t valor_a_guardar_en_swap);
long get_espacio_libre_en_swap();
void llenar_espacios_libres_swap();

#endif /* SWAP_H_ */
