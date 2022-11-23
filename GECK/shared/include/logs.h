#ifndef LOGGERS_H_
#define LOGGERS_H_

#include "shared.h"
#include "paquetes.h"

extern t_log* logger;
extern t_log* logger_debug;

void crear_loggers(char* module_name, t_log **logger_prod, t_log **logger_debug, t_config* config);

void log_list_inst(t_list*);
void log_lista_seg(t_list*);
void log_lista_tamanios_seg(t_list*);
void log_pcb(PCB*);
void log_cambio_de_estado(int id, t_estado_proceso anterior, t_estado_proceso nuevo);

char* str_ins(t_ins);
char* str_estado(t_estado_proceso);
char* str_registro(reg_cpu reg);
char* str_dispositivos(reg_cpu disp);
char* str_algoritmo(t_algoritmo_planificacion algoritmo);

#endif /* LOGGERS_H_ */
