#include "../include/consola.h"

t_log* logger;

int main(int argc, char** argv) {
	t_list *instrucciones = list_create();
	t_list *lista_segmentos = list_create();

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

	//instrucciones = parsear_pseudocod(pseudo_path);
	int kernel_fd = procesar_config(config_path, &lista_segmentos);

	log_debug(logger, "kernel file descriptor: %d", kernel_fd);

	return liberar_memoria(logger, kernel_fd);
}


int procesar_config(char *config_path, t_list **lista_segmentos) {
	char **lista;
	t_config* config = config_create(config_path);

	if (config== NULL) {
		log_error(logger, "No se pudo abrir el archivo de configuracion en ese path");
		exit(EXIT_FAILURE);
	}

	char* ip_kernel = config_get_string_value(config, "IP_KERNEL");
	char* puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
	lista = config_get_array_value(config, "SEGMENTOS");

	log_debug(logger, "ip: %s", ip_kernel);
	log_debug(logger, "puerto: %s", puerto_kernel);

	int conexion_kernel = conect_to_kernel(ip_kernel, puerto_kernel);

	if(conexion_kernel == -1) {
		log_info(logger, "No se ha podido conectar con el KERNEL");
		exit(EXIT_FAILURE);
	}

	send_debug(conexion_kernel);

	log_info(logger, "Se ha conectado con el KERNEL exitosamente");
	config_destroy(config);

	return conexion_kernel;
}

int conect_to_kernel(char* ip, char* puerto) {
	log_info(logger, "Iniciando conexion con el Kernel - Puerto: %s - IP: %s", ip, puerto);
	return crear_conexion(ip, puerto);
}

int liberar_memoria(t_log* logg, int fd) {
	liberar_conexion(fd);
	log_destroy(logg);
	return EXIT_SUCCESS;
}
