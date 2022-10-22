

#include "../include/planificadorLargoPlazo.h"


extern t_log* logger;
extern t_configuracion_kernel *config;

extern t_list* procesosNew;
extern t_list* procesosReady;

void nuevoProceso(PCB* pcb) {
	list_add(procesosNew, pcb);
	log_info(logger, "Se agrego un proceso de id: %d a la cola de NEW", pcb->id);

}

void pasarAReady() {
	if(list_size(procesosReady) < config->grado_max_multiprogramacion) {
		PCB* pcb = list_get(procesosNew, 0);
		list_add(procesosReady, pcb);
		log_info(logger, "Cambio de estado New a Ready: “Cola Ready, con algoritmo: <<%s>> el proceso de id: %d”",config->algoritmo_planificacion, pcb->id);
	}
}

/*
void finalizarProceso(int processId) {
	PCB* proceso = list_find(procesosNew, )
	list_remove(procesosNew, proceso );
}
*/
