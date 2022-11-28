#ifndef SUSPENDIDOS_H_
#define SUSPENDIDOS_H_

#include <shared.h>
#include <paquetes.h>
#include <semaphore.h>
#include <logs.h>
#include "configuracion.h"
#include "planificadorLargoPlazo.h"
#include <servidor.h>

#include <memoria_shared.h>

typedef struct {
	PCB* pcb;
	int tiempo_de_suspension;
	dispositivos dispositivo;
} t_manejar_block;

typedef struct {
	PCB* pcb;
	uint32_t reg;
} t_manejar_block_consola;

void suspender_proceso(void* void_args);
void ejecutar_suspension_en_hilo(PCB* pcb, int tiempo, dispositivos dispositivo);
void manejar_suspension_por(dispositivos dispo, PCB* pcb, int cliente_socket);

void ejecutar_bloqueo_page_fault(PCB* pcb, int segmento_solicitado, int pagina_solicitada);
void hilo_page_fault(void* void_args);

void op_teclado(int pid, reg_cpu reg, uint32_t valor);
void op_pantallla(int pid, reg_cpu reg);

void manejar_wait_dispositivo(dispositivos dispositivo);
void manejar_post_dispositivo(dispositivos dispositivo);

void mover_proceso_block_ready_by_pid(int pid);

#endif /* SUSPENDIDOS_H_ */
