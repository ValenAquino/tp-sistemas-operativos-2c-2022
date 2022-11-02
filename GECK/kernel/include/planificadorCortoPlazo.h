#ifndef PLANIFICADORCORTOPLAZO_H_
#define PLANIFICADORCORTOPLAZO_H_

#include "kernel.h"

void pasarAExec(PCB* pcb);

PCB* get_siguiente_proceso();

PCB* siguiente_proceso_FIFO();
PCB* siguiente_proceso_RR();
PCB* siguiente_proceso_FEEDBACK();

#endif /* PLANIFICADORCORTOPLAZO_H_ */
