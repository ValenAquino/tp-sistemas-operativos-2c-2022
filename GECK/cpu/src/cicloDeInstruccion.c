#include "../include/cicloDeInstruccion.h"

extern t_log *logger;
extern t_log *logger_debug;

extern t_configuracion_cpu *config;

extern int FLAG_FIN_QUANTUM;
extern int memoria_fd;

extern sem_t sem_respuesta_memoria;

extern uint32_t REG_AX;
extern uint32_t REG_BX;
extern uint32_t REG_CX;
extern uint32_t REG_DX;

int kernel_fd;
bool se_devolvio_pcb = false;

void ciclo_de_instruccion(PCB *pcb, int kernel_socket) {
	kernel_fd = kernel_socket;
	se_devolvio_pcb = false;

	log_trace(logger_debug, "Comenzando ciclo de instruccion");
	log_registros();

	ts_ins *ins_a_ejecutar = fetch(pcb);

	ins_a_ejecutar = decode(ins_a_ejecutar);

	execute(ins_a_ejecutar, pcb);

	check_interrupt(pcb);

	if (!se_devolvio_pcb)
		ciclo_de_instruccion(pcb, kernel_fd);
}

ts_ins* fetch(PCB *pcb) {
	log_trace(logger_debug, "FETCH: ");
	return list_get(pcb->instrucciones, pcb->programCounter);
}

ts_ins* decode(ts_ins *instruccion) {
	log_trace(logger_debug, "DECODE");

	switch (instruccion->name) {
	case SET:
	case ADD:
		log_trace(logger_debug,
				"Ejecutando un retardo de instruccion de: %d ms",
				config->retardo_instruccion);

		sleep(config->retardo_instruccion / 1000); // Sleep recibe tiempo en segundos

		log_trace(logger_debug, "FIN retardo de instruccion");
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

void execute(ts_ins *instruccion, PCB *pcb) {
	log_trace(logger_debug, "EXECUTE: ");

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
		success = execute_mov(instruccion, pcb, MOV_IN);
		break;
	case MOV_OUT:
		success = execute_mov(instruccion, pcb, MOV_OUT);
		break;
	}

	if (success == EXIT_SUCCESS)
		pcb->programCounter = pcb->programCounter + 1;
}

int execute_set(ts_ins *instruccion, PCB *pcb) {
	log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s> - <%d>", pcb->id,
			str_ins(instruccion->name), str_registro(instruccion->param1),
			instruccion->param2);

	return guardar_en_reg(instruccion->param1, instruccion->param2);
}

int execute_add(ts_ins *instruccion, PCB *pcb) {
	log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s> - <%s>", pcb->id,
			str_ins(instruccion->name), str_registro(instruccion->param1),
			str_registro(instruccion->param2));

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

int execute_io(ts_ins *instruccion, PCB *pcb) {
	op_code codigo;

	pcb->programCounter = pcb->programCounter + 1;
	actualizar_pcb(pcb);

	switch (instruccion->param1) {
	case PANTALLA:
		codigo = OP_PANTALLA;

		log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s> - <%s>", pcb->id,
				 str_ins(instruccion->name), str_registro(instruccion->param1),
				 str_registro(instruccion->param2) );
		break;

	case TECLADO:
		codigo = OP_TECLADO;

		log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s> - <%s>", pcb->id,
				 str_ins(instruccion->name), str_registro(instruccion->param1),
				 str_registro(instruccion->param2) );
		break;
	default:
		codigo = IO_KERNEL;

		log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%d> - <%d>", pcb->id,
				 str_ins(instruccion->name), instruccion->param1, instruccion->param2);

		break;
	}

	log_trace(logger_debug, "ENVIANDO PCB A KERNEL POR I/O: ");

	enviar_pcb(pcb, kernel_fd, codigo);

	if(codigo == OP_TECLADO || codigo == OP_PANTALLA) {
		enviar_registro(kernel_fd, instruccion->param2);
	}
	else {
		enviar_valor(kernel_fd, instruccion->param1); // dispositivo
		enviar_valor(kernel_fd, instruccion->param2); // unidades de trabajo
	}

	free(pcb);

	se_devolvio_pcb = true;
	return EXIT_FAILURE;
}

int execute_exit(ts_ins *instruccion, PCB *pcb) {
	log_info(logger, "PID: <%d> - Ejecutando: <%s>", pcb->id,
			str_ins(instruccion->name));

	eliminar_entradas_tlb_by_pid(pcb->id);

	pcb->programCounter = pcb->programCounter + 1;
	actualizar_pcb(pcb);

	log_trace(logger_debug, "ENVIANDO PCB A KERNEL POR EXIT");
	enviar_pcb(pcb, kernel_fd, FIN_POR_EXIT);

	free(pcb);
	se_devolvio_pcb = true;

	return EXIT_FAILURE;
}

int execute_mov(ts_ins *instruccion, PCB *pcb, t_ins inst) {
	log_mov(pcb->id, instruccion);

	dir_t dir_parcial = traducir_direccion(
			inst == MOV_IN ? instruccion->param2 : instruccion->param1,
			pcb->tablaSegmentos);

	if (hay_segmentation_fault(dir_parcial.nro_seg, pcb))
		return EXIT_FAILURE;

	int marco = pedir_marco_memoria(pcb->id, dir_parcial, memoria_fd);

	if (hay_page_fault(marco, pcb, dir_parcial))
		return EXIT_FAILURE;

	(inst == MOV_IN)
		? mov_in(pcb->id, dir_parcial, instruccion->param1)  // inst == MOV_IN
		: mov_out(pcb->id,dir_parcial, instruccion->param2); // inst != MOV_IN

	return EXIT_SUCCESS;
}

void mov_in(int pid, dir_t dir_parcial, uint32_t reg) {
	int valor_leido = leer_de_memoria(pid, dir_parcial);
	guardar_en_reg(reg, valor_leido);
}

void mov_out(int pid, dir_t dir_parcial, uint32_t reg) {
	escribir_en_memoria(pid, dir_parcial, reg);
}

void check_interrupt(PCB *pcb) {
	log_trace(logger_debug, "CHECK INTERRUPT\n");

	if (se_devolvio_pcb) {
		FLAG_FIN_QUANTUM = 0;
		return;
	}

	if (FLAG_FIN_QUANTUM) {
		FLAG_FIN_QUANTUM = 0;

		log_debug(logger_debug, "<FLAG_FIN_QUANTUM>\n");

		log_trace(logger_debug,
				"ENVIANDO PCB A KERNEL POR: FIN DE QUANTUM PID: <%d>", pcb->id);

		actualizar_pcb(pcb);
		enviar_pcb(pcb, kernel_fd, DESALOJO_QUANTUM);
		se_devolvio_pcb = true;
		return;
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

int guardar_en_reg(reg_cpu reg, int valor) {
	switch (reg) {
	case AX:
		REG_AX = valor;
		break;
	case BX:
		REG_BX = valor;
		break;
	case CX:
		REG_CX = valor;
		break;
	case DX:
		REG_DX = valor;
		break;
	default:
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void actualizar_pcb(PCB *pcb) {
	pcb->registros[AX] = REG_AX;
	pcb->registros[BX] = REG_BX;
	pcb->registros[CX] = REG_CX;
	pcb->registros[DX] = REG_DX;
}

void restaurar_contexto_ejecucion(uint32_t registros[]) {
	REG_AX = registros[AX];
	REG_BX = registros[BX];
	REG_CX = registros[CX];
	REG_DX = registros[DX];
}

int hay_segmentation_fault(int nro_seg, PCB *pcb) {
	if (nro_seg != SEG_FAULT_ERROR)
		return 0;

	log_trace(logger_debug,
			"ENVIANDO PCB A KERNEL POR: Segmentation Fault PID: <%d>", pcb->id);

	enviar_pcb(pcb, kernel_fd, SEGMENTATION_FAULT);
	se_devolvio_pcb = true;
	return 1;
}

int hay_page_fault(int marco, PCB *pcb, dir_t dir_parcial) {
	if (marco != PAGE_FAULT_ERROR)
		return 0;

	actualizar_pcb(pcb);

	log_trace(logger_debug,
			"ENVIANDO PCB A KERNEL POR PAGE_FAULT: nro_pag = %d, nro_seg = %d",
			dir_parcial.nro_pag, dir_parcial.nro_seg);

	enviar_pcb(pcb, kernel_fd, PAGE_FAULT_CPU);
	enviar_valor(kernel_fd, dir_parcial.nro_seg);
	enviar_valor(kernel_fd, dir_parcial.nro_pag);

	free(pcb);
	se_devolvio_pcb = true;
	return 1;
}

void log_registros() {
	log_debug(logger, "[AX: %u, BX: %u, CX: %u, DX: %u]", REG_AX, REG_BX,
			REG_CX, REG_DX);
}

void log_mov(int pid, ts_ins *instruccion) {

	if (instruccion->name == MOV_IN) {
		log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%s> - <%d>", pid,
				str_ins(instruccion->name), str_registro(instruccion->param1),
				instruccion->param2);
	} else {
		log_info(logger, "PID: <%d> - Ejecutando: <%s> - <%d> - <%s>", pid,
				str_ins(instruccion->name), instruccion->param1,
				str_registro(instruccion->param2));
	}
}
