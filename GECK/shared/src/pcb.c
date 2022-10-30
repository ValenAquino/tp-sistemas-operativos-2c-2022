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

    log_info(logger, "Se crea el proceso <%d> en NEW", id);
    return pcb;
}
