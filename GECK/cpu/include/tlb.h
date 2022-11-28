#ifndef TLB_H_
#define TLB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <shared.h>
#include <logs.h>
#include <semaphore.h>
#include <pthread.h>

#include <commons/temporal.h>
#include <commons/log.h>

#include "configuracion.h"

#define FORMATO_TIMESTAMP "%H:%M:%S:%MS"

typedef struct {
	int pid;
	int nro_seg;
	int nro_pag;
	int frame;
	time_t ult_ref_timestamp;
} entrada_tlb;

typedef enum {
	TLB_LRU,
	TLB_FIFO
} algoritmo_reemplazo_tlb;

/**
 * @NAME: ingresar_a_tlb
 * @DESC: Se encarga de agregar la entrada a la TLB
 * resolviendo el reemplazo de ser necesario.
 */
void ingresar_a_tlb(entrada_tlb *entrada);

/**
 * @NAME: buscar_marco_en_tlb
 * @DESC: Recibe el PID, numero de segmento y numero de pagina
 * y retorna el marco de memoria si se encontro la table en tlb (HIT)
 * o -1 si no se encontro (MISS)
 */
int buscar_marco_en_tlb(int pid, int nro_seg, int nro_pag);

// Handlers
void manejar_reemplazo_para(entrada_tlb *entrada);
void manejar_reemplazo_LRU(entrada_tlb *entrada_nueva);
void manejar_reemplazo_FIFO(entrada_tlb *entrada_nueva);

// Lista TLB
void remover_de_tlb_by_index(int indice);
void agregar_a_tlb(entrada_tlb *entrada);
void reemplazar_tlb(entrada_tlb *entrada_nueva, int indice_victima);
void limpiar_tlb();

uint32_t buscar_indice_en_tlb(entrada_tlb *entrada_buscada);
entrada_tlb* buscar_entrada_por_seg_y_pag(int nro_seg, int nro_pag);

// LRU
entrada_tlb* buscar_victima_lru();
void* comparar_ultimas_referencias(entrada_tlb *entrada_uno,
		entrada_tlb *entrada_dos);

// Entradas
int son_entradas_iguales(entrada_tlb *entrada_uno, entrada_tlb *entrada_dos);
entrada_tlb* crear_entrada_tlb(int pid, int nro_seg, int nro_pag, int frame);
void eliminar_entradas_tlb_by_pid(int pid);

// Utils
algoritmo_reemplazo_tlb get_algoritmo_reemplazo(char *algoritmo);
void loggear_tlb();

#endif /* TLB_H_ */
