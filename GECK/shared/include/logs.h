#ifndef LOGGERS_H_
#define LOGGERS_H_

#include "shared.h"
#include "paquetes.h"

extern t_log* logger;

void log_list_inst(t_list*);
void log_lista_seg(t_list*);
void log_pcb(PCB* pcb);

#endif /* LOGGERS_H_ */
