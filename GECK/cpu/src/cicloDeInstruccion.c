#include "../include/cicloDeInstruccion.h"

extern t_log* logger;
extern t_configuracion_cpu *config;
extern int FLAG_INTERRUPT;
extern t_list* tiempos_io;

// Anotacion:
// capaz que acá conviene un array y que se acceda con el enum como indice: regisros[REG_AX]
// Así no hay tantos switches para saber cual registro agarrar
//
// Además hay que limpiar un poco el código porque me concentré en intentar que funcione

extern uint32_t REG_AX;
extern uint32_t REG_BX;
extern uint32_t REG_CX;
extern uint32_t REG_DX;

int kernel_fd;

bool inicializados = false;
bool se_devolvio_pcb = false;

void inicializar_registro() {
	inicializados = true;
	REG_AX = 0;
	REG_BX = 0;
	REG_CX = 0;
	REG_DX = 0;
}

void log_registros() {
	log_trace(logger, "Imprimiento registros");
	log_debug (
		logger,
		"[AX: %u, BX: %u, CX: %u, DX: %u]",
		REG_AX, REG_BX, REG_CX, REG_DX
	);
}

void ciclo_de_instruccion(PCB* pcb, int kernel_socket) {
	kernel_fd = kernel_socket;
	se_devolvio_pcb = false;
	
	log_trace(logger, "Antes de ejecutar");
	log_pcb(pcb);

	log_registros();

	log_trace(logger, "FETCH");
	ts_ins* ins_a_ejecutar = fetch(pcb);
	
	log_trace(logger, "DECODE");
	ins_a_ejecutar = decode(ins_a_ejecutar);
	
	log_trace(logger, "EXECUTE");
	execute(ins_a_ejecutar, pcb);

	log_trace(logger, "CHECK INTERRUPT\n\n");
	check_interrupt(pcb);
}

ts_ins *fetch(PCB* pcb) {
	return list_get(pcb->instrucciones, pcb->programCounter);
}

ts_ins* decode(ts_ins* instruccion) { 
	// No debería ser una funcion void?
	switch (instruccion->name) {
		case SET:
		case ADD:
			log_trace(logger, "Ejecutando un retardo de instruccion de: %d ms", config->retardo_instruccion);
			sleep(config->retardo_instruccion / 1000); // Sleep recibe tiempo en segundos
			log_trace(logger, "FIN retardo de instruccion");
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
	log_info(
		logger, 
		"PID: <%d> - Ejecutando: <%s> - <%d> - <%d>", 
		pcb->id, str_ins(instruccion->name), instruccion->param1, instruccion->param2
	);

	int success;
	switch (instruccion->name) {
		case SET:
			success = execute_set(instruccion, pcb);
			break;
		case ADD:
			success = execute_add(instruccion, pcb);
			break;
		case IO:
			success = execute_io(instruccion, pcb);
			break;
		case EXIT:
			success = execute_exit(instruccion, pcb);
			break;
		case MOV_IN:
			success = execute_mov_in(instruccion, pcb);
			break;
		case MOV_OUT:
			success = execute_mov_out(instruccion, pcb);
			break;
	}

	if (success == EXIT_SUCCESS) { // exit succes = 0 = false 
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
		// Para mi habría que loggearlo y finalizar todos los módulos
		// En la consigna no especifica nada
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

int execute_io(ts_ins* instruccion, PCB *pcb) {
	pcb->programCounter = pcb->programCounter + 1;

	actualizar_pcb(pcb);
	log_trace(logger, "ENVIANDO PCB A KERNEL POR I/O");
	
	switch(instruccion->param1){
		case DISCO:
			enviar_pcb(pcb, kernel_fd, OP_DISCO);
			enviar_codop(kernel_fd, instruccion->param2);
			break;
		case IMPRESORA:
			enviar_pcb(pcb, kernel_fd, OP_IMPRESORA);
			enviar_codop(kernel_fd, instruccion->param2);
			break;
		case PANTALLA:
			break;
		case TECLADO:
			break;
	}
	

	se_devolvio_pcb = true;
	return EXIT_FAILURE;
}

int execute_exit(ts_ins* instruccion, PCB *pcb) {
	pcb->programCounter = pcb->programCounter + 1;
	
	actualizar_pcb(pcb);
	log_trace(logger, "ENVIANDO PCB A KERNEL POR EXIT");
	enviar_pcb(pcb, kernel_fd, FIN_POR_EXIT);
	
	free(pcb);
	se_devolvio_pcb = true;

	return EXIT_FAILURE;
}

int execute_mov_in(ts_ins* instruccion, PCB *pcb) {
	return EXIT_SUCCESS;
}

int execute_mov_out(ts_ins* instruccion, PCB *pcb) {
	return EXIT_SUCCESS;
}

void check_interrupt(PCB* pcb) {
	if(FLAG_INTERRUPT) {
		actualizar_pcb(pcb);
		enviar_pcb(pcb, kernel_fd, FIN_POR_EXIT);
		return;
	}

	if(!se_devolvio_pcb) {
		ciclo_de_instruccion(pcb, kernel_fd);
	}
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

void actualizar_pcb(PCB* pcb) {
	pcb->registros[AX] = REG_AX;
	pcb->registros[BX] = REG_BX;
	pcb->registros[CX] = REG_CX;
	pcb->registros[DX] = REG_DX;	
}
