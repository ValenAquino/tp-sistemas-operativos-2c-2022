#ifndef PLANIFICADORLARGOPLAZO_H_
#define PLANIFICADORLARGOPLAZO_H_

#include <commons/collections/list.h>
#include <commons/string.h>
#include <pcb.h>
#include "kernel.h"

void nuevoProceso(PCB* pcb);
void pasarAReady();

// PRUEBA
void dispatch_pcb(PCB* pcb);

void* serializar_pcb(PCB* pcb);

#endif /* PLANIFICADORLARGOPLAZO_H_ */
