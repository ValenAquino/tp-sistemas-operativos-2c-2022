#include "../include/cicloDeInstruccion.h"

extern t_log* logger;
extern t_configuracion_cpu *config;

extern uint32_t REG_AX;
extern uint32_t REG_BX;
extern uint32_t REG_CX;
extern uint32_t REG_DX;

ts_ins *fetch(PCB* pcb) {
	return list_get(pcb->instrucciones, pcb->programCounter);
}

ts_ins* decode(ts_ins* instruccion) {
	switch (instruccion->name) {
		case SET:
		case ADD:
			sleep(config->retardo_instruccion);
			return instruccion;
		case MOV_IN:
		case MOV_OUT:
		case IO:
		case EXIT:
			return instruccion;
		default:
			//TODO: manejar error de instruccion
			exit(EXIT_FAILURE);
			break;
	}
}

void execute(ts_ins* instruccion, PCB *pcb) {

	log_info(logger, "PID: <%d> - Ejecutando: <%d> - <%d> - <%d>", pcb->id, instruccion->name, instruccion->param1, instruccion->param2);
	int success;
	switch (instruccion->name) {
		case SET:
			success = execute_set(instruccion, pcb);
			break;
		case ADD:
			execute_add(instruccion, pcb);
			break;
		case IO:
			execute_io(instruccion, pcb);
			break;
		case EXIT:
			execute_exit(instruccion, pcb);
			break;
		case MOV_IN:
			execute_mov_in(instruccion, pcb);
			break;
		case MOV_OUT:
			execute_mov_out(instruccion, pcb);
			break;
	}

	if (success) {
		pcb->programCounter = pcb->programCounter + 1;
	}
}

int execute_set(ts_ins* instruccion, PCB *pcb) {
	switch (instruccion->param1) { // Estamos supiendo que en SET el primer parametro es el registro
	case AX:
		REG_AX = instruccion->param2;
		break;
	case BX:
		REG_BX = instruccion->param2;
		break;
	case CX:
		REG_CX = instruccion->param2;
		break;
	case DX:
		REG_DX = instruccion->param2;
		break;
	default:
		//TODO: Manejar error de instruccion;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int execute_add(ts_ins* instruccion, PCB *pcb) {
	switch (instruccion->param1) { // Estamos supiendo que en SET el primer parametro es el registro
	case AX:
		REG_AX = REG_AX + get_valor_registro(instruccion->param2);
		break;
	case BX:
		REG_BX = REG_BX + get_valor_registro(instruccion->param2);
		break;
	case CX:
		REG_CX = REG_CX + get_valor_registro(instruccion->param2);
		break;
	case DX:
		REG_DX = REG_DX + get_valor_registro(instruccion->param2);
		break;
	default:
		//TODO: Manejar error de instruccion;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

uint32_t get_valor_registro(reg_cpu registro) {
	switch (registro) {
	case AX:
		return REG_AX;
	case BX:
		return REG_BX;
	case CX:
		return REG_CX;
	case DX:
		return REG_DX;
	}
	exit(EXIT_FAILURE);
}

int execute_io(ts_ins* instruccion, PCB *pcb) {
	return EXIT_FAILURE;
}

int execute_exit(ts_ins* instruccion, PCB *pcb) {
	return EXIT_FAILURE;
}

int execute_mov_in(ts_ins* instruccion, PCB *pcb) {
	return EXIT_FAILURE;
}

int execute_mov_out(ts_ins* instruccion, PCB *pcb) {
	return EXIT_FAILURE;
}
