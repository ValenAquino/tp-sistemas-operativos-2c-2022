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
