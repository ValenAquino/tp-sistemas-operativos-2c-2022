#include "../include/logs.h"

void crear_loggers(char* module_name, t_log **logger_prod, t_log **logger_debug, int mostrar_logs) {
	char* file_name_debug = malloc(sizeof(module_name) + sizeof("-debug.log"));
	char* logger_debug_name = malloc(sizeof(module_name) + sizeof("-DEBUG"));
	char* file_name = malloc(sizeof(module_name) + sizeof(".log"));

	int mostrar_logs_tp;
	int mostrar_logs_debug;

	switch (mostrar_logs) { // 0 -> SOLO LOGS OBLIGATORIOS / 1 -> SOLO LOGS DEBUG Y OTRAS YERBAS / 2 -> AMBOS
	case 0:
		mostrar_logs_tp = 1;
		mostrar_logs_debug = 0;
		break;
	case 1:
		mostrar_logs_tp = 0;
		mostrar_logs_debug = 1;
		break;
	case 2:
	default:
		mostrar_logs_tp = 1;
		mostrar_logs_debug = 1;
		break;
	}

	sprintf(file_name_debug, "%s-debug.log", module_name);
	sprintf(logger_debug_name, "%s-DEBUG", module_name);
	sprintf(file_name, "%s.log", module_name);

	*logger_debug = log_create(file_name_debug, logger_debug_name, mostrar_logs_debug, LOG_LEVEL_TRACE);
	*logger_prod = log_create(file_name, module_name, mostrar_logs_tp, LOG_LEVEL_INFO);

	free(file_name_debug);
	free(logger_debug_name);
	free(file_name);
}

void log_list_inst(t_list* instrucciones) {
	for(int i = 0; i < list_size(instrucciones); i++) {
		ts_ins *inst = list_get(instrucciones, i);

		log_debug(
			logger,
			"Instruccion = [n: %d, p1: %d, p2: %d]",
			inst->name, inst->param1, inst->param2
		);
	}
}

void log_lista_seg(t_list* tablaSegmentos) {
	for(int i = 0; i < list_size(tablaSegmentos); i++) {
		int *seg = list_get(tablaSegmentos, i);
		log_debug(logger, "segmento[%d] = %d", i, *seg);
	}
}

void log_pcb(PCB* pcb) {
	t_list* instrucciones = pcb->instrucciones;
	t_list* segmentos = pcb->tablaSegmentos;

    log_trace(logger, "IMPRIMIENDO PCB");
	log_debug(logger, "ID: %d, PC: %d", pcb->id, pcb->programCounter);
	log_debug(logger, "STATE: %d, FD: %d", pcb->estado_actual, pcb->socket_consola);

	log_debug (
		logger,
		"[AX: %u, BX: %u, CX: %u, DX: %u]",
		pcb->registros[0], pcb->registros[1], pcb->registros[2], pcb->registros[3]
	);

	log_list_inst(instrucciones);
	log_lista_seg(segmentos);
	
    log_trace(logger, "FIN PCB");
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

char* str_dispositivos(reg_cpu disp) {
	switch (disp) {
	case DISCO:
		return "DISCO";
	case IMPRESORA:
		return "IMPRESORA";
	case TECLADO:
		return "TECLADO";
	case PANTALLA:
		return "PANTALLA";
	default:
		return "UNKNOWN";
	}
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

    log_error(logger, "El algoritmo es incorrecto");
    exit(EXIT_FAILURE);
}
