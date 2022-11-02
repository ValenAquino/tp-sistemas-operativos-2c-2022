#include "../include/planificadorLargoPlazo.h"

extern t_log* logger;
extern t_configuracion_kernel *config;

extern t_list* procesosNew;
extern t_list* procesosReady;
extern t_list* procesosExit;

extern int cpu_dispatch_fd;

void planificador_largo_plazo() {
	while(1)

	return;
}

void nuevoProceso(PCB* pcb) { // cambiar a pasarANew()
	list_add(procesosNew, pcb);
	log_info(logger, "Se agrego un proceso de id: %d a la cola de NEW", pcb->id);

	pasarAReady();
}

void imprimir_ready() {
	int size = list_size(procesosReady);
	char *pids = string_new();

	for (int i = 0; i < size-1; i++) {
		PCB *pcb = list_get(procesosReady, i);
		string_append(&pids, string_itoa(pcb->id));
		string_append(&pids, ", ");
	}

	PCB *pcb = list_get(procesosReady, size-1);
	string_append(&pids, string_itoa(pcb->id));

	log_info(
		logger, 
		"Cola Ready <%s>: [%s]", 
		get_algoritmo_string(config->algoritmo_planificacion), pids
	);

	free(pids);
}

void dispatch_pcb(PCB* pcb) {
	enviar_pcb(pcb, cpu_dispatch_fd, DISPATCH_PCB);
	free(pcb);
}

void pasarAReady() {
	log_trace(
		logger, 
		"procesos ready: %d, grado multi: %d", 
		list_size(procesosReady), config->grado_max_multiprogramacion
	);

	if(list_size(procesosReady) < config->grado_max_multiprogramacion) {
		PCB* pcb = list_remove(procesosNew, 0);
		pcb->estado_actual = READY_STATE;
		list_add(procesosReady, pcb);

		log_cambio_de_estado(pcb->id, NEW_STATE, READY_STATE);
		imprimir_ready();
	}
}

void pasarAExit(PCB* pcb) {
	pcb->estado_actual = EXIT_STATE;
	log_cambio_de_estado(pcb->id, EXEC_STATE, EXIT_STATE);

	enviar_codop(pcb->socket_consola, FIN_POR_EXIT);
	
	list_add(procesosExit, pcb);
	free(pcb);
}
