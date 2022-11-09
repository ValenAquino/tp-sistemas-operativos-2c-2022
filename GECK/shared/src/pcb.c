#include "../include/pcb.h"

extern t_log* logger;

PCB* nuevoPcb(int id, int fd_consola, t_list* instr, t_list* tablaSegmentos) {
    PCB* pcb = (PCB*) malloc(sizeof(PCB));
    pcb->id = id;
    pcb->socket_consola = fd_consola;
    pcb->estado_actual = NEW_STATE;
    pcb->instrucciones = instr;
    pcb->programCounter = 0;
    pcb->tablaSegmentos = tablaSegmentos;
    pcb->registros[0] = 0;
    pcb->registros[1] = 0;
    pcb->registros[2] = 0;
    pcb->registros[3] = 0;

    log_info(logger, "Se crea el proceso <%d>", id);
    return pcb;
}

PCB *obtener_proceso_por_pid(int pid, t_list* lista, sem_t mutex) {
	bool get_element(void *element) {
		PCB *elementPcb = element;
		return elementPcb->id == pid;
	}

	if (pid == -1)
		return NULL;

	sem_wait(&mutex);
	PCB *pcb = list_remove_by_condition(lista, get_element);
	sem_post(&mutex);
	return pcb;
}