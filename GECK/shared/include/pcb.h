#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdint.h>
#include <semaphore.h>
#include <pthread.h>

typedef enum {
	NEW_STATE, READY_STATE, EXEC_STATE, BLOCK_STATE, EXIT_STATE
} t_estado_proceso;

typedef struct PCB {
	int socket_consola;
	t_estado_proceso estado_actual;
	int id;
	int programCounter;
	uint32_t registros[4];
	t_list *instrucciones;
	t_list *tablaSegmentos;
	t_list *tamanios_segmentos;
} PCB;

PCB* nuevoPcb(int id, int fd_consola, t_list *instr, t_list *tamanio_segmentos);
PCB* obtener_proceso_por_pid(int pid, t_list *lista, pthread_mutex_t mutex);
void destruir_pcb(PCB* pcb);

#endif /* PCB_H_ */
