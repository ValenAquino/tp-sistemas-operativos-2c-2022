#ifndef SUSPENDIDOS_H_
#define SUSPENDIDOS_H_

#include <shared.h>
#include <paquetes.h>
#include <semaphore.h>
#include <logs.h>
#include "configuracion.h"
#include "planificadorLargoPlazo.h"

typedef struct {
	PCB* pcb;
	int tiempo_de_suspension;
} t_manejar_block;

typedef struct {
	PCB* pcb;
	uint32_t reg;
} t_manejar_block_consola;

void suspender_proceso(void* void_args);
void ejecutar_suspension_en_hilo(PCB* pcb, int tiempo);
void manejar_suspension_por(dispositivos dispo, PCB* pcb, int cliente_socket);

void op_teclado(int pid, reg_cpu reg, uint32_t valor);
void op_pantallla(int pid, reg_cpu reg);

#endif /* SUSPENDIDOS_H_ */