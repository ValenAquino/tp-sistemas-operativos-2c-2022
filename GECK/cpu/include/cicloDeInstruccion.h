/*
 * cicloDeInstruccion.h
 *
 *  Created on: Oct 30, 2022
 *      Author: ubuntu
 */

#ifndef CICLODEINSTRUCCION_H_
#define CICLODEINSTRUCCION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>
#include <logs.h>

#include "comunicacion.h"
#include "configuracion.h"

ts_ins *fetch(PCB* pcb);
ts_ins* decode(ts_ins* instruccion);
void execute(ts_ins* instruccion, PCB *pcb);

int execute_set(ts_ins* instruccion, PCB *pcb);
int execute_add(ts_ins* instruccion, PCB *pcb);
int execute_io(ts_ins* instruccion, PCB *pcb);
int execute_exit(ts_ins* instruccion, PCB *pcb);
int execute_mov_in(ts_ins* instruccion, PCB *pcb);
int execute_mov_out(ts_ins* instruccion, PCB *pcb);

uint32_t get_valor_registro(reg_cpu registro);

#endif /* CICLODEINSTRUCCION_H_ */
