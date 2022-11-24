#ifndef CICLODEINSTRUCCION_H_
#define CICLODEINSTRUCCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <semaphore.h>
#include <shared.h>
#include <logs.h>

#include "comunicacion.h"
#include "configuracion.h"
#include "cpu.h"
#include "mmu.h"

void ciclo_de_instruccion(PCB* pcb, int kernel_socket);

ts_ins *fetch(PCB* pcb);
ts_ins* decode(ts_ins* instruccion);

void execute(ts_ins* instruccion, PCB *pcb);
int execute_set(ts_ins* instruccion, PCB *pcb);
int execute_add(ts_ins* instruccion, PCB *pcb);
int execute_io(ts_ins* instruccion, PCB *pcb);
int execute_exit(ts_ins* instruccion, PCB *pcb);
int execute_mov_in(ts_ins* instruccion, PCB *pcb);
int execute_mov_out(ts_ins* instruccion, PCB *pcb);

void check_interrupt();

uint32_t get_valor_registro(reg_cpu registro);
int guardar_en_reg(reg_cpu reg, int valor);
void actualizar_pcb(PCB* pcb);
void restaurar_contexto_ejecucion(uint32_t registros[]);

#endif /* CICLODEINSTRUCCION_H_ */
