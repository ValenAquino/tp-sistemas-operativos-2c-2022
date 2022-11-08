#ifndef PLANIFICADORCORTOPLAZO_H_
#define PLANIFICADORCORTOPLAZO_H_

#include "kernel.h"

typedef struct {
	PCB* pcb;
	int tiempo_de_suspension;
} t_manejar_block;

void pasarAExec(PCB* pcb);
void pasarABlock(PCB* pcb, dispositivos disp);

void planificador_corto_plazo();

bool filter_pcb_by_id(void* item);
PCB* remove_and_get_ready();

PCB* get_siguiente_proceso();
PCB* siguiente_proceso_FIFO();
PCB* siguiente_proceso_RR();
PCB* siguiente_proceso_FEEDBACK();

#endif /* PLANIFICADORCORTOPLAZO_H_ */
