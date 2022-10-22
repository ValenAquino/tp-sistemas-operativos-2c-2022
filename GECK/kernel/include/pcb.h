#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>

//enum operacion {EXIT, I_O, SET, ADD, MOV_IN, MOV_OUT};
/*
typedef struct instruccion
{
    enum operacion op;
    int parametro1, parametro2;
    struct instruccion* sig;

}instruccion;
*/
typedef struct PCB
{
    int id;
    t_list* instrucciones;
    int programCounter;

    //registrosCpu
    t_list* tablaSegmentos; //lo resuelve memoria a peticion del kernel

}PCB;

PCB* nuevoPcb(int id,  t_list* instr, t_list* tablaSegmentos);


#endif /* PCB_H_ */
