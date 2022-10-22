#include "../include/pcb.h"

//PCB nuevoPcb(int id, int tamanio, Instruccion instr, int prgCounter, float estimacion)
PCB* nuevoPcb(int id,  t_list* instr, t_list* tablaSegmentos)
{
    PCB* pcb = (PCB*) malloc(sizeof(PCB));
    //PCB pcb;
    pcb->id = id;
    pcb->instrucciones = instr;
    pcb->programCounter = 0;
    pcb->tablaSegmentos = tablaSegmentos;
    //pcb->registrosCpu...
    return pcb;
}
