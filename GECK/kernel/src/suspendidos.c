#include "../include/suspendidos.h"

extern t_configuracion_kernel* config;

extern t_list* procesosBlock;
extern pthread_mutex_t mutex_block;
extern sem_t sem_disco;
extern sem_t sem_impresora;

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

	PCB* pcb_obtenido = obtener_proceso_por_pid(pcb->id, procesosBlock, mutex_block);
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

void manejar_suspension_por(dispositivos dispo, PCB* pcb, int cliente_socket) {
	int unidades_de_trabajo = recibir_operacion(cliente_socket);
	int tiempo_por_unidad = obtener_tiempo_io(dispo, config->tiempos_io);
	int tiempo_de_suspension = tiempo_por_unidad * unidades_de_trabajo / 1000;
			
	pasarABlock(pcb, dispo);

	manejar_wait_dispositivo(dispo);

	log_trace(logger_debug, "SLEEP DE %d", tiempo_de_suspension);
	ejecutar_suspension_en_hilo(pcb, tiempo_de_suspension, dispo);
}

void manejar_wait_dispositivo(dispositivos dispositivo) {
	switch (dispositivo) {
		case IMPRESORA:
			sem_wait(&sem_impresora);
		break;
		case DISCO:
			sem_wait(&sem_disco);
		break;
		default:
			log_error(logger_debug, "Dispositivo desconocido. No se puede manejar la suspension.");
			exit(EXIT_FAILURE);
		break;
	}
}


void manejar_post_dispositivo(dispositivos dispositivo) {
	switch (dispositivo) {
		case IMPRESORA:
			sem_post(&sem_impresora);
		break;
		case DISCO:
			sem_post(&sem_disco);
		break;
		default:
			log_error(logger_debug, "Dispositivo desconocido. Error al hacer el post para desbloquear el dispositivo.");
			exit(EXIT_FAILURE);
		break;
	}
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

	t_manejar_page_fault* args = malloc(sizeof(t_manejar_page_fault));
	args->pcb = pcb;
	args->pagina_solicitada = pagina_solicitada;
	args->segmento_solicitado = segmento_solicitado;

	pthread_create(&hilo, NULL, (void*) hilo_page_fault, (void*) args);
	pthread_detach(hilo);
}

void hilo_page_fault(void* void_args) {
	t_manejar_page_fault* args = (t_manejar_page_fault*) void_args;
	int num_pagina = args->pagina_solicitada;
	int num_segmento = args->segmento_solicitado;
	PCB* pcb = args->pcb;
	free(args);

	pasarABlockPageFault(pcb);

	solicitar_pagina(pcb, memoria_fd, num_segmento, num_pagina);
}

