#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <shared.h>
#include <paquetes.h>
#include <logs.h>

#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/log.h>

typedef struct t_configuracion_kernel
{
    char *ip_memoria;
    char *ip_kernel;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_cpu_dispatch;
    char *puerto_cpu_interrupt;
    char *puerto_kernel;
    char *puerto_escucha;
    t_algoritmo_planificacion algoritmo_planificacion;
    int grado_max_multiprogramacion;
    int quantum_rr;
    t_list* tiempos_io;
} t_configuracion_kernel;

t_configuracion_kernel* procesar_config(t_config *nuevo_config);
t_algoritmo_planificacion procesar_algoritmo(char* algoritmo);
t_list* string_to_int(char **segmentos);

void test_read_config(t_configuracion_kernel* config);
t_list* procesar_tiempos_io(char **tiempos, char **dispositivos);

int obtener_tiempo_io(dispositivos disp_que_busco, t_list* lista_dispositivos);
void liberar_configuracion_kernel(t_configuracion_kernel* config_kernel);

#endif /* CONFIGURACION_H_ */
