#include "../include/memoria.h"

t_log* logger;
t_log* logger_debug;
t_configuracion_memoria* config;

char* config_path;

FILE* area_swap;

uint32_t memoria_disponible;
void* memoria_principal;

t_list *tablas_de_paginas;
t_list *espacios_libres_en_swap;

pthread_mutex_t tablas_de_paginas_mutex;
pthread_mutex_t memoria_p_mutex;
pthread_mutex_t swap_mutex;

int main(int argc, char** argv) {
	if (argc < 2) {
		config_path = "memoria.config";
	} else {
		config_path = argv[1];
	}

	iniciar_memoria();
	log_debug(logger_debug, "config: %s", config_path);
	int memoria_fd = crear_conexion(config->ip_memoria, config->puerto_escucha);

	while (server_memoria(SERVERNAME, memoria_fd));

	fclose(area_swap);
	/// TODO: LIBERAR MEMORIA.
	// CERRAR ARCHIVO DE SWAP

	return EXIT_SUCCESS;
}

void iniciar_memoria() {
	t_config *config_file = abrir_configuracion(config_path);
	crear_loggers("memoria", &logger, &logger_debug, config_file);
	config = procesar_config(config_file);

	test_read_config(config);

	tablas_de_paginas = list_create();
	espacios_libres_en_swap = list_create();

	pthread_mutex_init(&memoria_p_mutex, NULL);
	pthread_mutex_init(&swap_mutex, NULL);
	pthread_mutex_init(&tablas_de_paginas_mutex, NULL);

	crear_archivo_swap();
	llenar_espacios_libres_swap();
}

int crear_conexion(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger_debug, "%s lista para recibir al cliente", SERVERNAME);

	return server_fd;
}

int cargar_memoria(t_configuracion_memoria* config) {
    memoria_principal = malloc(config->tam_memoria);   // void*
    if (memoria_principal == NULL) {
        log_error(logger, "Fallo en el malloc a memoria_principal");
        return EXIT_FAILURE;
    }
    memset(memoria_principal, 0, config->tam_memoria);
    memoria_disponible = config->tam_memoria;

    return EXIT_SUCCESS;
}
