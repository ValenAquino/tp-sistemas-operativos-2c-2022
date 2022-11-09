#include "../include/suspendidos.h"

extern t_log* logger;
extern t_configuracion_kernel* config;

extern t_list* procesosBlock;
extern sem_t planificar;
extern sem_t mutex_block;

// IO usando sleep

void suspender_proceso(void* void_args ) {
	t_manejar_block* args = (t_manejar_block*) void_args;
	int tiempo = args->tiempo_de_suspension;
	PCB* pcb = args->pcb;
	free(args);

	sleep(tiempo);
	pasarAReady(pcb);
}

void ejecutar_suspension_en_hilo(PCB* pcb, int tiempo) {
    pthread_t hilo;

    t_manejar_block* args = malloc(sizeof(t_manejar_block));
    args->pcb = pcb;
    args->tiempo_de_suspension = tiempo;

    pthread_create(&hilo, NULL, (void*) suspender_proceso, (void*) args);
    pthread_detach(hilo);
}

void manejar_suspension_por(dispositivos dispo, PCB* pcb, int cliente_socket) {
	int unidades_de_trabajo = recibir_operacion(cliente_socket);
	int tiempo_por_unidad = obtener_tiempo_io(dispo, config->tiempos_io);
	int tiempo_de_suspension = tiempo_por_unidad * unidades_de_trabajo / 1000;
			
	pasarABlock(pcb, dispo);

	log_trace(logger, "SLEEP DE %d", tiempo_de_suspension);
	ejecutar_suspension_en_hilo(pcb, tiempo_de_suspension);
}

// IO con consola

void op_teclado(int pid, reg_cpu reg, uint32_t valor) {
	PCB* pcb = obtener_proceso_por_pid(pid, procesosBlock, mutex_block);

	log_debug(logger, "[registro: %s, respuesta_teclado: [%d]", str_registro(reg), valor);
	
	pcb->registros[reg] = valor;
	
	pasarAReady(pcb);
}

void op_pantallla(int pid, reg_cpu reg) {
	PCB* pcb = obtener_proceso_por_pid(pid, procesosBlock, mutex_block);
	log_debug(logger, "registro: [%s] impreso por Proceso: <%d>", str_registro(reg), pcb->id);

	pasarAReady(pcb);
}