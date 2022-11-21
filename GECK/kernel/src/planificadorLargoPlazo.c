#include "../include/planificadorLargoPlazo.h"

extern t_log* logger;
extern t_log* logger_debug;

extern t_configuracion_kernel *config;

extern t_list* procesosNew;
extern t_list* procesosReady;
extern t_list* procesosBajaPrioridad;
extern t_list* procesosExit;

extern int cpu_dispatch_fd;

extern sem_t sem_procesos_ready;
extern sem_t sem_proceso_nuevo;
extern sem_t mutex_ready;
extern sem_t planificar;
extern sem_t mutex_baja_prioridad;

void planificador_largo_plazo() {

	while(1) {
		sem_wait(&sem_proceso_nuevo);
		sem_wait(&sem_procesos_ready);
		PCB* pcb = list_remove(procesosNew, 0);
		pasarAReady(pcb, false);
	}
	return;
}

void pasarANew(PCB* pcb) {
	list_add(procesosNew, pcb);
	log_info(logger_debug, "Se agrego un proceso de id: %d a la cola de NEW", pcb->id);
	sem_post(&sem_proceso_nuevo);
	// CREAR HILO DE QUANTUM.

}

void imprimir_ready(t_list* lista, char* nombreLista) {
	int size = list_size(lista);
	char *pids = string_new();

	if (size == 0) {
		log_info(
			logger,
			"Cola Ready %s <%s>: []",
			nombreLista, str_algoritmo(config->algoritmo_planificacion)
		);
		return;
	}

	for (int i = 0; i < size-1; i++) {
		PCB *pcb = list_get(lista, i);
		string_append(&pids, string_itoa(pcb->id));
		string_append(&pids, ", ");
	}

	PCB *pcb = list_get(lista, size-1);
	string_append(&pids, string_itoa(pcb->id));

	log_info(
		logger, 
		"Cola Ready %s <%s>: [%s]",
		nombreLista, str_algoritmo(config->algoritmo_planificacion), pids
	);

	free(pids);
}

void dispatch_pcb(PCB* pcb) {
	enviar_pcb(pcb, cpu_dispatch_fd, DISPATCH_PCB);
	free(pcb);
}

void pasarAReady(PCB* pcb, bool desalojado_por_quantum) {
	switch (config->algoritmo_planificacion) {
	case FIFO:
	case RR:
			sem_wait(&mutex_ready);
			list_add(procesosReady, pcb);
			imprimir_ready(procesosReady, "");
			sem_post(&mutex_ready);
			break;
	case FEEDBACK:
		if(desalojado_por_quantum){
			sem_wait(&mutex_baja_prioridad);
			list_add(procesosBajaPrioridad, pcb);
			sem_post(&mutex_baja_prioridad);
		}
		else {
			sem_wait(&mutex_ready);
			list_add(procesosReady, pcb);
			sem_post(&mutex_ready);
		}

		sem_wait(&mutex_ready);
		imprimir_ready(procesosReady, "RR");
		sem_post(&mutex_ready);

		sem_wait(&mutex_baja_prioridad);
		imprimir_ready(procesosBajaPrioridad, "FIFO");
		sem_post(&mutex_baja_prioridad);
		break;
	}

	log_cambio_de_estado(pcb->id, pcb->estado_actual, READY_STATE);
	pcb->estado_actual = READY_STATE;
	sem_post(&planificar);
}

void pasarAExit(PCB* pcb) {
	pcb->estado_actual = EXIT_STATE;
	log_cambio_de_estado(pcb->id, EXEC_STATE, EXIT_STATE);
	enviar_codop(pcb->socket_consola, FIN_POR_EXIT);
	list_add(procesosExit, pcb);
	sem_post(&sem_procesos_ready);
	//free(pcb); // lo estamos enlistando y liberando, perdemos ese espacio de memoria
}
