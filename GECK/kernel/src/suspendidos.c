#include "../include/suspendidos.h"

extern t_configuracion_kernel* config;

extern t_list* procesosBlock;
extern pthread_mutex_t mutex_block;

extern int memoria_fd;

// IO usando sleep

void suspender_proceso(void* void_args) {
	t_manejar_block* args = (t_manejar_block*) void_args;
	int tiempo = args->tiempo_de_suspension;
	PCB* pcb = args->pcb;
	dispositivos dispositivo = args->dispositivo;
	free(args);

	sleep(tiempo);
	manejar_post_dispositivo(dispositivo);

	mover_proceso_block_ready_by_pid(pcb->id);
}

void mover_proceso_block_ready_by_pid(int pid) {
	// OJO QUE obtener_proceso_por_pid HACE UN REMOVE DE LA LISTA QUE LE PASAMOS.
	PCB* pcb_obtenido = obtener_proceso_por_pid(pid, procesosBlock, mutex_block);
	pasarAReady(pcb_obtenido, false);
}

void ejecutar_suspension_en_hilo(PCB* pcb, int tiempo, dispositivos dispositivo) {
    pthread_t hilo;

    t_manejar_block* args = malloc(sizeof(t_manejar_block));
    args->pcb = pcb;
    args->tiempo_de_suspension = tiempo;
    args->dispositivo = dispositivo;

    pthread_create(&hilo, NULL, (void*) suspender_proceso, (void*) args);
    pthread_detach(hilo);
}

void manejar_suspension_por(int indice_dispo, PCB* pcb, int cliente_socket) {
	int unidades_de_trabajo = recibir_valor(cliente_socket);
	int tiempo_por_unidad = obtener_tiempo_io(indice_dispo, config->dispositivos);
	int tiempo_de_suspension = tiempo_por_unidad * unidades_de_trabajo / 1000;
			
	pasarABlock(pcb, indice_dispo);

	manejar_wait_dispositivo(indice_dispo);

	log_trace(logger_debug, "SLEEP DE %d", tiempo_de_suspension);
	ejecutar_suspension_en_hilo(pcb, tiempo_de_suspension, indice_dispo);
}

void manejar_wait_dispositivo(int indice_disp) {
	ts_dispositivo *dispositivo = list_get(config->dispositivos, indice_disp);

	sem_wait(&dispositivo->sem_dispositivo);
}

void manejar_post_dispositivo(int indice_disp) {
	ts_dispositivo *dispositivo = list_get(config->dispositivos, indice_disp);

	sem_post(&dispositivo->sem_dispositivo);
}

// IO con consola

void op_teclado(int pid, reg_cpu reg, uint32_t valor) {
	PCB* pcb = obtener_proceso_por_pid(pid, procesosBlock, mutex_block);

	log_debug(logger_debug, "[registro: %s, respuesta_teclado: [%d]", str_registro(reg), valor);
	
	pcb->registros[reg] = valor;
	
	pasarAReady(pcb, false);
}

void op_pantallla(int pid, reg_cpu reg) {
	PCB* pcb = obtener_proceso_por_pid(pid, procesosBlock, mutex_block);
	log_debug(logger_debug, "registro: [%s] impreso por Proceso: <%d>", str_registro(reg), pcb->id);

	pasarAReady(pcb, false);
}

// PAGE FAULT
void ejecutar_bloqueo_page_fault(PCB* pcb, int segmento_solicitado, int pagina_solicitada) {
	pthread_t hilo;

	segmento_t* seg = list_get(pcb->tablaSegmentos, segmento_solicitado);

	t_manejar_page_fault* args = malloc(sizeof(t_manejar_page_fault));
	args->pcb = pcb;
	args->pagina_solicitada = pagina_solicitada;
	args->nro_segmento_solicitado = segmento_solicitado;
	args->indice_tablas_de_paginas = seg->indice_tablas_paginas;

	pthread_create(&hilo, NULL, (void*) hilo_page_fault, (void*) args);
	pthread_detach(hilo);
}

void hilo_page_fault(void* void_args) {
	t_manejar_page_fault* args = (t_manejar_page_fault*) void_args;

	pasarABlockPageFault(args->pcb);

	// args se libera en solicitar_pagina
	solicitar_pagina(memoria_fd, args);
}

