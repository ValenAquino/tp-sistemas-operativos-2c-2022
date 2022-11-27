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
int execute_mov(ts_ins *instruccion, PCB *pcb, t_ins inst);
void mov_in(int desplazamiento_pag, uint32_t reg);
void mov_out(uint32_t reg);

void check_interrupt();

int hay_segmentation_fault(int nro_seg, PCB* pcb);
int hay_page_fault(int marco, PCB* pcb, dir_t dir_parcial);

void log_registros();
void log_mov(int pid, ts_ins *instruccion);

uint32_t get_valor_registro(reg_cpu registro);
int guardar_en_reg(reg_cpu reg, int valor);
void actualizar_pcb(PCB* pcb);
void restaurar_contexto_ejecucion(uint32_t registros[]);

#endif /* CICLODEINSTRUCCION_H_ */
