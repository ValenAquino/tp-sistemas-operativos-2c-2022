#include "../include/planificadorCortoPlazo.h"

extern t_list *procesosReady;
extern t_configuracion_kernel *config;

void pasarAExec(PCB* pcb) {
	pcb->estado_actual = EXEC_STATE;
	log_cambio_de_estado(pcb->id, READY_STATE, EXEC_STATE);

	dispatch_pcb(pcb);
}

PCB* get_siguiente_proceso() {
	switch (config->algoritmo_planificacion) {
		case FIFO:
			return siguiente_proceso_FIFO();
		case RR:
			return siguiente_proceso_RR();
		case FEEDBACK:
			return siguiente_proceso_FEEDBACK();
	}

	log_error(logger, "Algoritmo de planificacion invalido.");
	exit(EXIT_FAILURE);
}

PCB* siguiente_proceso_FIFO() {
	PCB* pcb = list_remove(procesosReady, 0);
	return pcb;
}

PCB* siguiente_proceso_RR() {
	return NULL;
}

PCB* siguiente_proceso_FEEDBACK() {
	return NULL;
}



