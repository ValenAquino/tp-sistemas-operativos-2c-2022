#include "../include/consola.h"

t_log* logger;

int main(int argc, char** argv) {
	char **array_segmentos;
	int tiempo_pantalla;

	// Level trace para que hagamos logs debugs y trace
	// Así los logs minimos quedan al level info
	logger = log_create("Consola.log", "logger", true, LOG_LEVEL_TRACE);

	if (argc < 3) {
		log_error(logger, "Se necesitan mas argumentos para inicializar correctamente la consola");
		return EXIT_FAILURE;
	}

	char* config_path = argv[1];
	char* pseudo_path = argv[2];

	log_debug(logger, "config: %s", config_path);
	log_debug(logger, "pseudo: %s", pseudo_path);

	t_list *instrucciones = parsear_pseudocod(pseudo_path);	

	log_debug(logger, "list size: %d", list_size(instrucciones));
	for(int i = 0; i < list_size(instrucciones); i++) {
		ts_instr *instasd = (ts_instr *) list_get(instrucciones, i);
		log_debug(logger, "Instruccion: %d, p1: %d, p2: %d", instasd->instr, instasd->param1, instasd->param2);
	}

	t_config* config = procesar_config(config_path, &array_segmentos, &tiempo_pantalla);
	int kernel_fd = connect_to_kernel(config);

	log_debug(logger, "kernel file descriptor: %d", kernel_fd);

	return liberar_memoria(logger, kernel_fd, config);
}

t_config* procesar_config(char *config_path, char ***array_segmentos, int* tiempo_pantalla) {
	t_config* config = config_create(config_path);

	if (config== NULL) {
		log_error(logger, "No se pudo abrir el archivo de configuracion en ese path");
		exit(EXIT_FAILURE);
	}

	*tiempo_pantalla = config_get_int_value(config, "TIEMPO_PANTALLA");
	*array_segmentos = config_get_array_value(config, "SEGMENTOS");

	log_info(logger, "tiempo_pantalla: %d", *tiempo_pantalla);

	for(int i=0; array_segmentos[i] != NULL; i++)
		log_debug(logger, "segmento[%d]: %s", i, (*array_segmentos)[i]);

	return config;
}

int connect_to_kernel(t_config* config) {
	char* ip = config_get_string_value(config, "IP_KERNEL");
	char* puerto = config_get_string_value(config, "PUERTO_KERNEL");

	log_info(logger, "Iniciando conexion con el Kernel - Puerto: %s - IP: %s", ip, puerto);
	int kernel_fd = crear_conexion(ip, puerto);

	if(kernel_fd == -1) {
		log_error(logger, "No se ha podido iniciar la conexion con el kernel");
		exit(EXIT_FAILURE);
	}

	return kernel_fd;
}

t_instr procesar_inst(char* inst) {
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
}

reg_cpu leer_reg(char *registro) {
	if(strcmp("AX", registro) == 0)
		return AX;

	if(strcmp("BX", registro) == 0)
		return BX;

	if(strcmp("CX", registro) == 0)
		return CX;

	if(strcmp("DX", registro) == 0)
		return DX;
}

dispositivos leer_dispositivo(char *dispositivo) {
	if(strcmp("DISCO", dispositivo) == 0)
		return DISCO;

	if(strcmp("TECLADO", dispositivo) == 0)
		return TECLADO;

	if(strcmp("PANTALLA", dispositivo) == 0)
		return PANTALLA;
}

void leer_instruccion(FILE **f_pseudo, ts_instr *inst) {
	char buffer[10];

	fscanf(*f_pseudo, "%s", buffer);
	inst->instr = procesar_inst(buffer);
}

void leer_parametros(FILE **f_pseudo, ts_instr *inst) {
	char buffer[10];

	switch(inst->instr) {
	case SET:
		fscanf(*f_pseudo, "%s", buffer);
		inst->param1 = leer_reg(buffer);
		fscanf(*f_pseudo, "%d", &(inst->param2));
		break;

	case ADD:
		fscanf(*f_pseudo, "%s", buffer);
		inst->param1 = leer_reg(buffer);
		fscanf(*f_pseudo, "%s", buffer);
		inst->param2 = leer_reg(buffer);
		break;

	case MOV_IN:
		fscanf(*f_pseudo, "%s", buffer);
		inst->param1 = leer_reg(buffer);
		fscanf(*f_pseudo, "%d", &((inst->param2)));
		break;

	case MOV_OUT:
		fscanf(*f_pseudo, "%d", &(inst->param1));
		fscanf(*f_pseudo, "%s", buffer);
		inst->param2 = leer_reg(buffer);
		break;

	case IO:
		fscanf(*f_pseudo, "%s", buffer);
		inst->param1 = leer_dispositivo(buffer);

		if(inst->param1 == DISCO)
			fscanf(*f_pseudo, "%d", &(inst->param2));
		else
			inst->param2 = leer_reg(buffer);

		break;

	case EXIT:
		inst->param1 = -1; // los inicializo para que no tengan basura
		inst->param2 = -1; // si leemos exit no hay que leer sus param
		break;

	default:
		log_error(logger, "Se ha recibido una instruccion incorrecta");
		exit(EXIT_FAILURE);
		break;
	}
}

ts_instr* crear_instruccion(FILE **f_pseudo) {
	ts_instr *inst = malloc(sizeof(ts_instr));

	leer_instruccion(f_pseudo, inst);
	leer_parametros(f_pseudo, inst);

	return inst;
}

t_list* parsear_pseudocod(char* pseudo_path) {
	ts_instr *inst, *instasd;
	FILE* f_pseudo = fopen(pseudo_path, "r");
	t_list* lista_inst = list_create();

	if(f_pseudo == NULL) {
		log_error(logger, "No se ha podido abrir el archivo de pseudocodigo");
		exit(EXIT_FAILURE);
	}

	while(!feof(f_pseudo)) {
		inst = crear_instruccion(&f_pseudo);
		list_add(lista_inst, inst);
		//log_debug(logger, "Instruccion: %d, p1: %d, p2: %d", inst.instr, inst.param1, inst.param2);
	}

	fclose(f_pseudo);

	return lista_inst;
}

int liberar_memoria(t_log* logg, int fd, t_config* config) {
	liberar_conexion(fd);
	config_destroy(config);
	log_destroy(logg);
	return EXIT_SUCCESS;
}
