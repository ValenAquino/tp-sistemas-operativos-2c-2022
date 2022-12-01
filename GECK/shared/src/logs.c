#include "../include/logs.h"

void crear_loggers(char* module_name, t_log **logger_prod, t_log **logger_debug, t_config* config) {
	char **mostrar_logs_char = config_get_array_value(config, "MOSTRAR_LOGS");
	t_list *mostrar_logs = array_char_to_list_int(mostrar_logs_char);

	// TODO: Agregar validacion y default value si no existe la key en config.

	char* file_name_debug = malloc(sizeof(module_name) + sizeof("-debug.log"));
	char* logger_debug_name = malloc(sizeof(module_name) + sizeof("-DEBUG"));
	char* file_name = malloc(sizeof(module_name) + sizeof(".log"));

	int* mostrar_logs_tp = (int *) list_get(mostrar_logs, 0);
	int* mostrar_logs_debug = (int *) list_get(mostrar_logs, 1);

	sprintf(file_name_debug, "%s-debug.log", module_name);
	sprintf(logger_debug_name, "%s-DEBUG", module_name);
	sprintf(file_name, "%s.log", module_name);

	*logger_debug = log_create(file_name_debug, logger_debug_name, *mostrar_logs_debug, LOG_LEVEL_TRACE);
	*logger_prod = log_create(file_name, module_name, *mostrar_logs_tp, LOG_LEVEL_INFO);

	free(file_name_debug);
	free(logger_debug_name);
	free(file_name);
}

void log_list_inst_consola(t_list* instrucciones) {
	for(int i = 0; i < list_size(instrucciones); i++) {
		ts_ins_consola *inst = list_get(instrucciones, i);

		log_debug(
			logger_debug,
			"Instruccion = [n: %s, p1: %s, p2: %s]",
			str_ins(inst->name), inst->param1, inst->param2
		);
	}
}

void log_list_inst(t_list* instrucciones) {
	for(int i = 0; i < list_size(instrucciones); i++) {
		ts_ins *inst = list_get(instrucciones, i);

		log_debug(
			logger_debug,
			"Instruccion = [%s, %d, %d]",
			str_ins(inst->name), inst->param1, inst->param2
		);
	}
}

void log_lista_tamanios_seg(t_list* tabla_tamanios) {
	for(int i = 0; i < list_size(tabla_tamanios); i++) {
		int *tamanio = list_get(tabla_tamanios, i);
		log_debug(logger_debug, "tamanio segmento [%d] = %d", i, *tamanio);
	}
}

void log_lista_seg(t_list* tabla_segmentos) {

	if (list_size(tabla_segmentos) == 0) {
		log_debug(logger_debug, "Todavia no hay segmentos en este proceso");
		return;
	}

	for(int i = 0; i < list_size(tabla_segmentos); i++) {
		segmento_t *seg = list_get(tabla_segmentos, i);
		log_debug(logger_debug, "segmento [%d] = { tam: %d, id_tabla_de_pags: %d }", i, seg->tamanio_segmento, seg->indice_tablas_paginas);
	}
}

void log_pcb(PCB* pcb) {
	t_list* instrucciones = pcb->instrucciones;
	t_list* segmentos = pcb->tablaSegmentos;
	t_list* tamanios_segmentos = pcb->tamanios_segmentos;

    log_trace(logger_debug, "IMPRIMIENDO PCB");
	log_debug(logger_debug, "ID: %d, PC: %d", pcb->id, pcb->programCounter);
	log_debug(logger_debug, "STATE: %d, FD: %d", pcb->estado_actual, pcb->socket_consola);

	log_debug (
		logger_debug,
		"[AX: %u, BX: %u, CX: %u, DX: %u]",
		pcb->registros[0], pcb->registros[1], pcb->registros[2], pcb->registros[3]
	);

	log_list_inst(instrucciones);
	log_lista_tamanios_seg(tamanios_segmentos);
	log_lista_seg(segmentos);
	
    log_trace(logger_debug, "FIN PCB");
}

char* str_ins(t_ins ins) {
	switch (ins) {
	case SET:
		return "SET";
	case ADD:
		return "ADD";
	case MOV_IN:
		return "MOV_IN";
	case MOV_OUT:
		return "MOV_OUT";
	case IO:
		return "IO";
	case EXIT:
		return "EXIT";
	
	default:
		return "UNKNOWN";
		break;
	}
	return "UNKNOWN";
}

char* str_estado(t_estado_proceso estado) {	
	switch (estado) {
	case NEW_STATE:
		return "NEW";
	case READY_STATE:
		return "READY";
	case EXEC_STATE:
		return "EXEC";
	case BLOCK_STATE:
		return "BLOCK";
	case EXIT_STATE:
		return "EXIT";
	default:
		return "UNKNONW";
	}
	return "UNKNONW";
}

char* str_dispositivos(int indice_disp, t_list *lista_dispositivos) {
	ts_dispositivo *dispositivo = list_get(lista_dispositivos, indice_disp);

	if(dispositivo != NULL)
		return dispositivo->nombre;
	else
		return "UNKNOWN";
}

char* str_registro(reg_cpu reg) {
	switch (reg) {
	case AX:
		return "AX";
	case BX:
		return "BX";
	case CX:
		return "CX";
	case DX:
		return "DX";
	default:
		return "UNKNOWN";
	}
	return "UNKNOWN";
}

void log_cambio_de_estado(int id, t_estado_proceso anterior, t_estado_proceso nuevo)  {
	log_info(
		logger, 
		"PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>",
		id, str_estado(anterior), str_estado(nuevo)
	);
}



char* str_algoritmo(t_algoritmo_planificacion algoritmo) {

	switch (algoritmo) {
	case FIFO:
		return "FIFO";
	case RR:
		return "RR";
	case FEEDBACK:
		return "FEEDBACK";
	}

    log_error(logger_debug, "El algoritmo es incorrecto");
    exit(EXIT_FAILURE);
}
