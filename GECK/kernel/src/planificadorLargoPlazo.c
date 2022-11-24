#include "../include/planificadorLargoPlazo.h"

extern t_log* logger;
extern t_log* logger_debug;

extern t_configuracion_kernel *config;

extern t_list* procesosNew;
extern t_list* procesosReady;
extern t_list* procesosBajaPrioridad;
extern t_list* procesosExit;

extern int cpu_dispatch_fd;
extern int memoria_fd;

extern sem_t sem_procesos_ready;
extern sem_t sem_proceso_nuevo;
extern sem_t sem_estructuras_memoria;

extern pthread_mutex_t mutex_ready;
extern sem_t planificar;
extern pthread_mutex_t mutex_baja_prioridad;

void planificador_largo_plazo() {

	while(1) {
		sem_wait(&sem_proceso_nuevo);
		sem_wait(&sem_procesos_ready);
		PCB* pcb = list_get(procesosNew, 0);
		enviar_solicitud_crear_estructuras_memoria(pcb->tamanios_segmentos, memoria_fd, pcb->id);
		sem_wait(&sem_estructuras_memoria);
		log_pcb(pcb);
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
		pthread_mutex_lock(&mutex_ready);
		list_add(procesosReady, pcb);
		imprimir_ready(procesosReady, "");
		pthread_mutex_unlock(&mutex_ready);
		break;
	case FEEDBACK:
		if(desalojado_por_quantum){
			pthread_mutex_lock(&mutex_baja_prioridad);
			list_add(procesosBajaPrioridad, pcb);
			pthread_mutex_unlock(&mutex_baja_prioridad);
		}
		else {
			pthread_mutex_lock(&mutex_ready);
			list_add(procesosReady, pcb);
			pthread_mutex_unlock(&mutex_ready);
		}

		pthread_mutex_lock(&mutex_ready);
		imprimir_ready(procesosReady, "RR");
		pthread_mutex_unlock(&mutex_ready);

		pthread_mutex_lock(&mutex_baja_prioridad);
		imprimir_ready(procesosBajaPrioridad, "FIFO");
		pthread_mutex_unlock(&mutex_baja_prioridad);
		break;
	}

	log_cambio_de_estado(pcb->id, pcb->estado_actual, READY_STATE);
	pcb->estado_actual = READY_STATE;

	// TODO: Aca se estaria enviando cada vez que entra a ready.
	// Hay que revisar si hay que implementar alguna logica para ver si es necesario volver a crear las estructuras en memoria

	sem_post(&planificar);
}

void pasarAExit(PCB* pcb) {
	pcb->estado_actual = EXIT_STATE;
	log_cambio_de_estado(pcb->id, EXEC_STATE, EXIT_STATE);
	enviar_codop(pcb->socket_consola, FIN_POR_EXIT);
	list_add(procesosExit, pcb);
	sem_post(&sem_procesos_ready);
}
