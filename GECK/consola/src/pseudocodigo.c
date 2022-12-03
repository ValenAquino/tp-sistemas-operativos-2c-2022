#include "../include/pseudocodigo.h"

extern t_log* logger_debug;

t_list* parsear_pseudocod(char* pseudo_path) {
	ts_ins_consola *inst;

	FILE* f_pseudo = fopen(pseudo_path, "r");
	t_list* lista_inst = list_create();

	if(f_pseudo == NULL) {
		log_error(logger_debug, "No se ha podido abrir el archivo de pseudocodigo");
		exit(EXIT_FAILURE);
	}

	while(!feof(f_pseudo)) {
		inst = crear_instruccion(&f_pseudo);
		list_add(lista_inst, inst);

		if(inst->name == EXIT)
			break;
	}

	fclose(f_pseudo);

	log_list_inst_consola(lista_inst);

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

	log_error(logger_debug, "la instruccion '%s' es incorrecta", inst);
	exit(EXIT_FAILURE);
}

void leer_instruccion(FILE **f_pseudo, ts_ins_consola *inst) {
	char buffer[10];

	fscanf(*f_pseudo, "%s", buffer);
	inst->name = procesar_inst(buffer);
}

char *procesar_buffer(FILE **f_pseudo) {
	char buffer[30];

	fscanf(*f_pseudo, "%s", buffer);

	int length = strlen(buffer);

	char *cadena = malloc(length + size_caracter_fin_de_cadena);
	strcpy(cadena, buffer);

	return cadena;
}

void leer_parametros(FILE **f_pseudo, ts_ins_consola *inst) {
	switch(inst->name) {
	case SET:
	case ADD:
	case MOV_IN:
	case MOV_OUT:
	case IO:
		inst->param1 = procesar_buffer(f_pseudo);
		inst->param2 = procesar_buffer(f_pseudo);
		break;

	case EXIT:
		char *cadena_vacia = "";
		inst->param1 = cadena_vacia;
		inst->param2 = cadena_vacia;
		break;

	default:
		log_error(logger_debug, "Se ha recibido una instruccion incorrecta");
		exit(EXIT_FAILURE);
		break;
	}
}

ts_ins_consola* crear_instruccion(FILE **f_pseudo) {
	ts_ins_consola *inst = malloc(sizeof(ts_ins_consola));

	leer_instruccion(f_pseudo, inst);
	leer_parametros(f_pseudo, inst);

	return inst;
}
