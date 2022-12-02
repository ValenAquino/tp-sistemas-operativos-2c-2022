#include "../include/consola.h"

t_log* logger;
t_log* logger_debug;
t_config* config;
int tiempo_pantalla;
int kernel_fd;

void sighandler(int x) {
	switch (x) {
	case SIGINT:
		liberar_memoria();
		exit(EXIT_SUCCESS);
		break;
	}
}

int main(int argc, char** argv) {
	signal(SIGINT, sighandler);
	t_list *lista_inst;
	t_list *lista_segmentos;

	if (argc < 3) {
		mostrar_mensaje_para_finalizar(
			"Se necesitan mas argumentos para inicializar correctamente la consola");
		return EXIT_FAILURE;
	}

	char* config_path = argv[1];
	char* pseudo_path = argv[2];

	config = abrir_configuracion(config_path);
	crear_loggers("consola", &logger, &logger_debug, config);

	log_debug(logger_debug, "config: %s", config_path);
	log_debug(logger_debug, "pseudo: %s", pseudo_path);

	lista_inst = parsear_pseudocod(pseudo_path);

	procesar_config(config, &lista_segmentos, &tiempo_pantalla);

	kernel_fd = connect_to_kernel(config);

	enviar_proceso(kernel_fd, lista_inst, lista_segmentos);

	t_manejar_conexion_args * comunicacion_args =
			malloc(sizeof(t_manejar_conexion_args));

	comunicacion_args->fd = kernel_fd;
	comunicacion_args->server_name = "CONSOLA - KERNEL";

	while(manejar_comunicacion(comunicacion_args));

	return liberar_memoria();
	return 0;
}

void procesar_config(t_config* config, t_list **lista_segmentos, int* tiempo_pantalla) {
	char **segmentos;

	*tiempo_pantalla = config_get_int_value(config, "TIEMPO_PANTALLA");
	segmentos = config_get_array_value(config, "SEGMENTOS");

	*lista_segmentos = array_char_to_list_int(segmentos);

	log_info(logger_debug, "tiempo_pantalla: %d", *tiempo_pantalla);
}

int connect_to_kernel(t_config* config) {
	char* ip = config_get_string_value(config, "IP_KERNEL");
	char* puerto = config_get_string_value(config, "PUERTO_KERNEL");

	log_info(logger_debug, "Iniciando conexion con el Kernel - Puerto: %s - IP: %s", ip, puerto);
	int kernel_fd = crear_conexion(ip, puerto);

	if(kernel_fd == -1) {
		log_error(logger_debug, "No se ha podido iniciar la conexion con el kernel");
		exit(EXIT_FAILURE);
	}

	send_debug(kernel_fd);
	log_debug(logger_debug, "kernel file descriptor: %d", kernel_fd);

	return kernel_fd;
}

int calcular_size_lista_ins(t_list* lista_inst) {
	int size = 0;

	for(int i = 0; i< list_size(lista_inst); i++) {
		ts_ins_consola *inst = list_get(lista_inst, i);

		int size_param1 = strlen(inst->param1) + size_caracter_fin_de_cadena;
		int size_param2 = strlen(inst->param2) + size_caracter_fin_de_cadena;

		size += size_param1 + size_param2 + 2*sizeof(int) + sizeof(t_ins);
	}

	return size;
}

void enviar_proceso(int kernel_fd, t_list* lista_inst, t_list* lista_segmentos) {
	ts_paquete* paquete = crear_paquete(ELEMENTOS_CONSOLA);

	int size_ins = calcular_size_lista_ins(lista_inst) + sizeof(int);
	int size_seg = sizeof(int) * list_size(lista_segmentos) + sizeof(int);

	void *ins = serializar_lista_ins_consola(lista_inst, size_ins);
	void *seg = serializar_lista_tamanios_seg(lista_segmentos, size_seg, 1);

	agregar_a_paquete(paquete, ins, size_ins);
	agregar_a_paquete(paquete, seg, size_seg);

	enviar_paquete(paquete, kernel_fd);
	eliminar_paquete(paquete);
}

int liberar_memoria() {
	log_trace(logger, "Finalizando consola");
	liberar_conexion(kernel_fd);
	config_destroy(config);
	log_destroy(logger_debug);
	log_destroy(logger);
	return EXIT_SUCCESS;
}
