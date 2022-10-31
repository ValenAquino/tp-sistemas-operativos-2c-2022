#ifndef PLANIFICADORLARGOPLAZO_H_
#define PLANIFICADORLARGOPLAZO_H_

#include <commons/collections/list.h>
#include <commons/string.h>
#include <pcb.h>
#include <cliente.h>
#include <protocolo.h>
#include "kernel.h"

#define str(x) #x

void nuevoProceso(PCB* pcb);
void pasarAReady();

// PRUEBA
void dispatch_pcb(PCB* pcb);

void* serializar_pcb(PCB* pcb);
void pasarAExit(PCB* pcb, int cliente_socket);

#endif /* PLANIFICADORLARGOPLAZO_H_ */
