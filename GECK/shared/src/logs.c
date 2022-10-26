#include "../include/logs.h"

void log_list_inst(t_list* instrucciones) {
	for(int i = 0; i < list_size(instrucciones); i++) {
		ts_ins *inst = list_get(instrucciones, i);

		log_debug(
			logger,
			"Instruccion = [n: %d, p1: %d, p2: %d]",
			inst->name, inst->param1, inst->param2
		);
	}
}

void log_lista_seg(t_list* tablaSegmentos) {
	for(int i = 0; i < list_size(tablaSegmentos); i++) {
		int *seg = list_get(tablaSegmentos, i);
		log_debug(logger, "segmento[%d] = %d", i, *seg);
	}
}

void log_pcb(PCB* pcb) {
	t_list* instrucciones = pcb->instrucciones;
	t_list* segmentos = pcb->tablaSegmentos;

    log_trace(logger, "IMPRIMIENDO PCB");
	log_debug(logger, "ID: %d, PC: %d", pcb->id, pcb->programCounter);

	log_debug (
		logger,
		"[AX: %u, BX: %u, DX: %u, CX: %u]",
		pcb->registros[0], pcb->registros[1], pcb->registros[2], pcb->registros[3]
	);

	log_list_inst(instrucciones);
	log_lista_seg(segmentos);
	
    log_trace(logger, "FIN PCB");
}
