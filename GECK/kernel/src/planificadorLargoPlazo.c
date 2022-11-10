#include "../include/planificadorLargoPlazo.h"

extern t_log* logger;
extern t_log* logger_debug;

extern t_configuracion_kernel *config;

extern t_list* procesosNew;
extern t_list* procesosReady;
extern t_list* procesosExit;

extern int cpu_dispatch_fd;

extern sem_t sem_procesos_ready;
extern sem_t sem_proceso_nuevo;
extern sem_t mutex_ready;
extern sem_t planificar;

void planificador_largo_plazo() {

	while(1) {
		sem_wait(&sem_proceso_nuevo);
		sem_wait(&sem_procesos_ready);
		PCB* pcb = list_remove(procesosNew, 0);
		pasarAReady(pcb);
	}

	return;
}

void pasarANew(PCB* pcb) {
	list_add(procesosNew, pcb);
	log_info(logger_debug, "Se agrego un proceso de id: %d a la cola de NEW", pcb->id);

	sem_post(&sem_proceso_nuevo);
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
		str_algoritmo(config->algoritmo_planificacion), pids
	);

	free(pids);
}

void dispatch_pcb(PCB* pcb) {
	enviar_pcb(pcb, cpu_dispatch_fd, DISPATCH_PCB);
	free(pcb);
}

void pasarAReady(PCB* pcb) {
	sem_wait(&mutex_ready);

	log_trace(
		logger_debug,
		"procesos ready: %d, grado multi: %d", 
		list_size(procesosReady), config->grado_max_multiprogramacion
	);

	list_add(procesosReady, pcb);
	imprimir_ready();

	sem_post(&mutex_ready);

	log_cambio_de_estado(pcb->id, pcb->estado_actual, READY_STATE);
	pcb->estado_actual = READY_STATE;
	sem_post(&planificar);
}

void pasarAExit(PCB* pcb) {
	pcb->estado_actual = EXIT_STATE;
	log_cambio_de_estado(pcb->id, EXEC_STATE, EXIT_STATE);
	enviar_codop(pcb->socket_consola, FIN_POR_EXIT);
	list_add(procesosExit, pcb);
	free(pcb); // lo estamos enlistando y liberando, perdemos ese espacio de memoria
}
