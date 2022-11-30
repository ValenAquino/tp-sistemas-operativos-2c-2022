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
void* leer_pagina_entera_de_swap(pagina_t* pagina, int pid, int nro_seg, int nro_pag);
void cargar_en_swap_pagina_entera(pagina_t* pagina, void* data_a_guardar_en_swap, int nro_seg, int nro_pag, int estaba_memoria_principal);
long get_espacio_libre_en_swap();
void llenar_espacios_libres_swap();
void ejecutar_retardo_swap();
void llenar_de_ceros_pagina_de_swap(pagina_t* pagina);

#endif /* SWAP_H_ */
