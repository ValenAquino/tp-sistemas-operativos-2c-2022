#include "../include/consola.h"

t_log* logger;

// TODO enviar los mensajes a kernel

int main(int argc, char** argv) {
	char* config_path;
	char* pseudo_path;
	t_config* config;
	t_list *lista_inst;
	t_list *lista_segmentos;
	int tiempo_pantalla;
	int kernel_fd;

	// Level trace para que hagamos logs debugs y trace
	// Así los logs minimos quedan al level info
	logger = log_create("Consola.log", "logger", true, LOG_LEVEL_TRACE);

	if (argc < 3) {
		log_error(logger, "Se necesitan mas argumentos para inicializar correctamente la consola");
		return EXIT_FAILURE;
	}

	config_path = argv[1];
	pseudo_path = argv[2];

	log_debug(logger, "config: %s", config_path);
	log_debug(logger, "pseudo: %s", pseudo_path);

	lista_inst = parsear_pseudocod(pseudo_path);	
	config = procesar_config(config_path, &lista_segmentos, &tiempo_pantalla);
	kernel_fd = connect_to_kernel(config);

	// TODO agregar logs para trackear que va pasando
	enviar_proceso(kernel_fd, lista_inst, lista_segmentos);

	t_manejar_conexion_args * comunicacion_args = malloc(sizeof(t_manejar_conexion_args));
	comunicacion_args->fd = kernel_fd;
	comunicacion_args->server_name = "CONSOLA - KERNEL";

	while(manejar_comunicacion(comunicacion_args));

	return liberar_memoria(logger, kernel_fd, config);;
}

t_list *char_to_int(char **segmentos) {
	t_list *lista_segmentos = list_create();

	for (int i = 0; segmentos[i] != NULL; i++) {
		int *seg = malloc(sizeof(int));
		*seg = atoi(segmentos[i]);
		
		list_add(lista_segmentos, seg);
	}

	return lista_segmentos;
}

t_config* procesar_config(char *config_path, t_list **lista_segmentos, int* tiempo_pantalla) {
	char **segmentos;
	t_config* config = config_create(config_path);

	if (config== NULL) {
		log_error(logger, "No se pudo abrir el archivo de configuracion en ese path");
		exit(EXIT_FAILURE);
	}

	*tiempo_pantalla = config_get_int_value(config, "TIEMPO_PANTALLA");
	segmentos = config_get_array_value(config, "SEGMENTOS");

	*lista_segmentos = char_to_int(segmentos);

	log_info(logger, "tiempo_pantalla: %d", *tiempo_pantalla);

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

	send_debug(kernel_fd);
	log_debug(logger, "kernel file descriptor: %d", kernel_fd);

	return kernel_fd;
}

void enviar_proceso(int kernel_fd, t_list* lista_inst, t_list* lista_segmentos) {
	ts_paquete* paquete = crear_paquete(ELEMENTOS_CONSOLA);

	int size_ins = sizeof(ts_ins) * list_size(lista_inst) + sizeof(int);
	int size_seg = sizeof(int) * list_size(lista_segmentos) + sizeof(int);

	log_list_inst(lista_inst);
	log_lista_seg(lista_segmentos);
	
	void *ins = serializar_lista_ins(lista_inst, size_ins);
	void *seg = serializar_lista_seg(lista_segmentos, size_seg);

	agregar_a_paquete(paquete, ins, size_ins);
	agregar_a_paquete(paquete, seg, size_seg);

	enviar_paquete(paquete, kernel_fd);
	eliminar_paquete(paquete);
}

int liberar_memoria(t_log* logg, int fd, t_config* config) {
	log_trace(logger, "Finalizando consola");
	liberar_conexion(fd);
	config_destroy(config);
	log_destroy(logg);
	return EXIT_SUCCESS;
}
