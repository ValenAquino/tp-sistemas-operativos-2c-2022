#include "../include/pcb.h"

extern t_log* logger;
extern t_log* logger_debug;

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

PCB *obtener_proceso_por_pid(int pid, t_list* lista, pthread_mutex_t mutex) {

	bool get_element(void *element) {
		PCB *elementPcb = element;
		return elementPcb->id == pid;
	}

	pthread_mutex_lock(&mutex);
	log_debug(logger_debug, "tamanio de la lista antes de remover item: %d", list_size(lista));
	PCB *pcb = list_remove_by_condition(lista, get_element);
	pthread_mutex_unlock(&mutex);

	return pcb;
}
