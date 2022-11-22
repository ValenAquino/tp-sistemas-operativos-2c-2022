#include "../include/planificadorCortoPlazo.h"

extern t_list *procesosReady;
extern t_list *procesosBajaPrioridad;
extern t_list *procesosBlock;

extern t_configuracion_kernel *config;

extern sem_t sem_procesos_ready; // GRADO DE MULTIPROGRAMACION
extern pthread_mutex_t mutex_ready; // PROTEGE LA LISTA DE READY
extern pthread_mutex_t mutex_baja_prioridad;  // PROTEGE LA LISTA DE READY-FIFO EN FEEDBACK.
extern pthread_mutex_t mutex_block;
extern sem_t planificar; // SINCRONIZA CORTO PLAZO
extern sem_t cpu_idle; // GRADO DE MULTIPROCESAMIENTO

int current_pcb_id;
extern bool esta_usando_rr;
extern bool volvio_pcb;

void pasarAExec(PCB* pcb) {
	pcb->estado_actual = EXEC_STATE;
	log_cambio_de_estado(pcb->id, READY_STATE, EXEC_STATE);
	dispatch_pcb(pcb);
	if (esta_usando_rr) {
		crear_hilo_quantum();
	}
}

void pasarABlock(PCB* pcb, dispositivos disp) {
	log_cambio_de_estado(pcb->id, pcb->estado_actual, BLOCK_STATE);
	pcb->estado_actual = BLOCK_STATE;

	pthread_mutex_lock(&mutex_block);
	list_add(procesosBlock, pcb);
	pthread_mutex_unlock(&mutex_block);

	log_info(logger, "PID: <%d> - Bloqueado por: <%s>", pcb->id, str_dispositivos(disp));
}

void pasarABlockPageFault(PCB* pcb) {
	log_cambio_de_estado(pcb->id, pcb->estado_actual, BLOCK_STATE);
	pcb->estado_actual = BLOCK_STATE;

	list_add(procesosBlock, pcb);
	log_info(logger, "PID: <%d> - Bloqueado por: <PAGE_FAULT>", pcb->id);
}

void planificador_corto_plazo() {
	while(1) {
		sem_wait(&planificar);
		sem_wait(&cpu_idle);
		PCB* pcb_a_ejecutar = get_siguiente_proceso();
		pasarAExec(pcb_a_ejecutar);
	}
	return;
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

	log_error(logger_debug, "Algoritmo de planificacion invalido.");
	exit(EXIT_FAILURE);
}

PCB* siguiente_proceso_FIFO() {
	pthread_mutex_lock(&mutex_ready);
	PCB* pcb = list_get(procesosReady, 0);
	pthread_mutex_unlock(&mutex_ready);
	
	return obtener_proceso_por_pid(pcb->id, procesosReady, mutex_ready);
}

bool filter_pcb_by_id(void* item) {
    PCB* pcb = (PCB*) item;
    return pcb->id == current_pcb_id;
}

PCB* remove_and_get_ready(t_list* lista_procesos, pthread_mutex_t mutex) {
	pthread_mutex_lock(&mutex);
	PCB* pcb = list_remove_by_condition(lista_procesos, filter_pcb_by_id);
	pthread_mutex_unlock(&mutex);
	return pcb;
}

PCB* siguiente_proceso_RR() {
	pthread_mutex_lock(&mutex_ready);
	PCB* pcb = list_get(procesosReady, 0);
	pthread_mutex_unlock(&mutex_ready);

	return obtener_proceso_por_pid(pcb->id, procesosReady, mutex_ready);
}

PCB* siguiente_proceso_FEEDBACK() {
	if(list_size(procesosReady) == 0){
		esta_usando_rr = false;
		pthread_mutex_lock(&mutex_baja_prioridad);
		PCB* pcb = list_get(procesosBajaPrioridad, 0);
		pthread_mutex_unlock(&mutex_baja_prioridad);
		return obtener_proceso_por_pid(pcb->id, procesosBajaPrioridad, mutex_block);
	} else {
		esta_usando_rr = true;
		return siguiente_proceso_RR();
	}
}
