#ifndef LOGGERS_H_
#define LOGGERS_H_

#include "shared.h"
#include "paquetes.h"

extern t_log* logger;

void log_list_inst(t_list*);
void log_lista_seg(t_list*);
void log_pcb(PCB*);
void log_cambio_de_estado(int id, t_estado_proceso anterior, t_estado_proceso nuevo);

char* str_ins(t_ins);
char* str_estado(t_estado_proceso);
char* str_registro(reg_cpu reg);
char* str_dispositivos(reg_cpu disp);

#endif /* LOGGERS_H_ */
