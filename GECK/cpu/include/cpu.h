#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <servidor.h>
#include <shared.h>

#define DISPATCH_SERVER_NAME "CPU_SERVER_DISPATCH"
#define INTERRUPT_SERVER_NAME "CPU_SERVER_INTERRUPT"

typedef struct t_configuracion_cpu
{
	int entradas_tlb; // Cantidad de entradas de la TLB
    int retardo_instruccion; // Tiempo en milisegundos que se deberá esperar al ejecutar las instrucciones SET y ADD.
    char* puerto_memoria; // Puerto al cual se deberá conectar con la Memoria
    char* puerto_escucha_dispatch; // Puerto en el cual se escuchará la conexión del Kernel para mensajes de dispatch
    char* puerto_escucha_interrupt; // Puerto en el cual se escuchará la conexión del Kernel para mensajes de interrupciones
    char *reemplazo_tlb; // Algoritmo de reemplazo para las entradas de la TLB. FIFO o LRU.
    char *ip_memoria; // IP a la cual se deberá conectar con la Memoria
    char *ip_cpu; // IP del servidor de CPU
} t_configuracion_cpu;

t_configuracion_cpu* procesar_config(char *);

void iniciar_cpu();
int crear_servidor(char* ip, char* puerto, char* server_name);
void test_read_config();
#endif /* CPU_H_ */
