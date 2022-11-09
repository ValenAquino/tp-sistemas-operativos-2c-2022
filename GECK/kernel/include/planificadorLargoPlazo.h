#ifndef PLANIFICADORLARGOPLAZO_H_
#define PLANIFICADORLARGOPLAZO_H_

#include <commons/collections/list.h>
#include <commons/string.h>
#include <pcb.h>
#include <cliente.h>
#include <protocolo.h>
#include "kernel.h"

void planificador_largo_plazo();
void pasarANew(PCB* pcb);

// PRUEBA
void dispatch_pcb(PCB* pcb);

void* serializar_pcb(PCB* pcb);
void pasarAExit(PCB* pcb);
void pasarAReady(PCB* pcb);

#endif /* PLANIFICADORLARGOPLAZO_H_ */
