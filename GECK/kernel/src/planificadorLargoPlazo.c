#include "../include/planificadorLargoPlazo.h"

extern t_log* logger;
extern t_configuracion_kernel *config;

extern t_list* procesosNew;
extern t_list* procesosReady;

extern int cpu_dispatch_fd;

void nuevoProceso(PCB* pcb) {
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

	log_info(logger, "Cola Ready <%s>: [%s]", config->algoritmo_planificacion, pids);
	free(pids);
}

void pasarAExec(PCB* pcb) {
	ts_paquete* paquete = crear_paquete(DISPATCH_PCB);

	int size_data = sizeof(int) * 2 + sizeof(uint32_t) * 4;
	int size_ins = sizeof(ts_ins) * list_size(pcb->instrucciones) + sizeof(int);
	int size_seg = sizeof(int) * list_size(pcb->tablaSegmentos) + sizeof(int);

	void* data = serializar_datos_pcb(pcb);
	void* inst = serializar_lista_ins(pcb->instrucciones, size_ins);
	void* segm = serializar_lista_seg(pcb->tablaSegmentos, size_seg);

	free(pcb);

	agregar_a_paquete(paquete, data, size_data);
	agregar_a_paquete(paquete, inst, size_ins);
	agregar_a_paquete(paquete, segm, size_seg);

	enviar_paquete(paquete, cpu_dispatch_fd);
	eliminar_paquete(paquete);
}

void dispatch_pcb(PCB* pcb) {
	// desencolar por id
	pasarAExec(pcb);
	// liberar memoria
	// algo mas?
}

void pasarAReady() {
	log_trace(logger, "procesos ready: %d, grado multi: %d", list_size(procesosReady), config->grado_max_multiprogramacion);

	if(list_size(procesosReady) < config->grado_max_multiprogramacion) {
		PCB* pcb = list_remove(procesosNew, 0);
		list_add(procesosReady, pcb);
		log_info(logger, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", pcb->id);
		imprimir_ready();
	}
}

/*
void finalizarProceso(int processId) {
	PCB* proceso = list_find(procesosNew, )
	list_remove(procesosNew, proceso );
}
*/
