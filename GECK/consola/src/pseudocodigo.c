#include "../include/pseudocodigo.h"

extern t_log* logger;

t_list* parsear_pseudocod(char* pseudo_path) {
	ts_ins *inst;
	FILE* f_pseudo = fopen(pseudo_path, "r");
	t_list* lista_inst = list_create();

	if(f_pseudo == NULL) {
		log_error(logger, "No se ha podido abrir el archivo de pseudocodigo");
		exit(EXIT_FAILURE);
	}

	while(!feof(f_pseudo)) {
		inst = crear_instruccion(&f_pseudo);
		list_add(lista_inst, inst);
		log_debug(logger, "Instruccion: %d, p1: %d, p2: %d", inst->name, inst->param1, inst->param2);
	}

	fclose(f_pseudo);

	return lista_inst;
}

t_ins procesar_inst(char* inst) {
	if(strcmp("SET", inst) == 0)
		return SET;

	if(strcmp("ADD", inst) == 0)
		return ADD;

	if(strcmp("MOV_IN", inst) == 0)
		return MOV_IN;

	if(strcmp("MOV_OUT", inst) == 0)
		return MOV_OUT;

	if(strcmp("I/O", inst) == 0)
		return IO;

	if(strcmp("EXIT", inst) == 0)
		return EXIT;

	log_error(logger, "la instruccion '%s' es incorrecta", inst);
	exit(EXIT_FAILURE);
}

reg_cpu procesar_reg(char *reg) {
	if(strcmp("AX", reg) == 0)
		return AX;

	if(strcmp("BX", reg) == 0)
		return BX;

	if(strcmp("CX", reg) == 0)
		return CX;

	if(strcmp("DX", reg) == 0)
		return DX;

	log_error(logger, "el registro '%s' es incorrecto", reg);
	exit(EXIT_FAILURE);
}

dispositivos procesar_dispositivo(char *disp) {
	if(strcmp("DISCO", disp) == 0)
		return DISCO;

	if(strcmp("TECLADO", disp) == 0)
		return TECLADO;

	if(strcmp("PANTALLA", disp) == 0)
		return PANTALLA;

	log_error(logger, "el dispositivo '%s' es incorrecto", disp);
	exit(EXIT_FAILURE);
}

void leer_instruccion(FILE **f_pseudo, ts_ins *inst) {
	char buffer[10];

	fscanf(*f_pseudo, "%s", buffer);
	inst->name = procesar_inst(buffer);
}

void leer_parametros(FILE **f_pseudo, ts_ins *inst) {
	char buffer[10];

	switch(inst->name) {
	case SET:
		fscanf(*f_pseudo, "%s", buffer);
		inst->param1 = procesar_reg(buffer);
		fscanf(*f_pseudo, "%d", &(inst->param2));
		break;

	case ADD:
		fscanf(*f_pseudo, "%s", buffer);
		inst->param1 = procesar_reg(buffer);
		fscanf(*f_pseudo, "%s", buffer);
		inst->param2 = procesar_reg(buffer);
		break;

	case MOV_IN:
		fscanf(*f_pseudo, "%s", buffer);
		inst->param1 = procesar_reg(buffer);
		fscanf(*f_pseudo, "%d", &((inst->param2)));
		break;

	case MOV_OUT:
		fscanf(*f_pseudo, "%d", &(inst->param1));
		fscanf(*f_pseudo, "%s", buffer);
		inst->param2 = procesar_reg(buffer);
		break;

	case IO:
		fscanf(*f_pseudo, "%s", buffer);
		inst->param1 = procesar_dispositivo(buffer);

		if(inst->param1 == DISCO)
			fscanf(*f_pseudo, "%d", &(inst->param2));
		else
			inst->param2 = procesar_reg(buffer);

		break;

	case EXIT:
		inst->param1 = -1; // los inicializo para que no tengan basura
		inst->param2 = -1; // 
		break;

	default:
		log_error(logger, "Se ha recibido una instruccion incorrecta");
		exit(EXIT_FAILURE);
		break;
	}
}

ts_ins* crear_instruccion(FILE **f_pseudo) {
	ts_ins *inst = malloc(sizeof(ts_ins));

	leer_instruccion(f_pseudo, inst);
	leer_parametros(f_pseudo, inst);

	return inst;
}
