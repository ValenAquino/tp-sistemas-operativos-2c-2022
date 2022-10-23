#ifndef PLANIFICADORLARGOPLAZO_H_
#define PLANIFICADORLARGOPLAZO_H_

#include <commons/collections/list.h>
#include "pcb.h"

#include "kernel.h"

void nuevoProceso(PCB* pcb);
void pasarAReady();


#endif /* PLANIFICADORLARGOPLAZO_H_ */
