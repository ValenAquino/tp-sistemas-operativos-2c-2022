#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdint.h>

typedef struct PCB
{
    int id;
    int programCounter;
    uint32_t registros[4];
    t_list* instrucciones;
    t_list* tablaSegmentos; //lo resuelve memoria a peticion del kernel
}PCB;

PCB* nuevoPcb(int id,  t_list* instr, t_list* tablaSegmentos);

#endif /* PCB_H_ */
