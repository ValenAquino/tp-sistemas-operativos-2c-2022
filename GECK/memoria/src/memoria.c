#include "../include/memoria.h"

t_log* logger;
t_log* logger_debug;
t_configuracion_memoria* config;

char* config_path;

uint32_t espacio_disponible_swap;
void* area_swap;

uint32_t memoria_disponible;
void* memoria_principal;

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

	return EXIT_SUCCESS;
}

void iniciar_memoria() {
	t_config *config_file = abrir_configuracion(config_path);
	crear_loggers("memoria", &logger, &logger_debug, config_file);
	config = procesar_config(config_file);
	test_read_config(config);
}

int crear_conexion(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger_debug, "%s lista para recibir al cliente", SERVERNAME);

	return server_fd;
}


int crear_archivo_swap(t_configuracion_memoria* config, char* path, uint32_t tamanio) {
    log_info(logger_debug, "Creando SWAP en <<%s>>", path);
    int fd_swap = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd_swap == -1) {
        log_error(logger_debug, "No se pudo crear el area de SWAP. (errno %i)", errno);
        return EXIT_FAILURE;
    }

    ftruncate(fd_swap, config->tamanio_swap);

    area_swap = mmap(NULL, config->tamanio_swap, PROT_READ | PROT_WRITE, MAP_SHARED, fd_swap, 0);
    if (errno!=0) log_error(logger_debug, "Error en mmap: errno %i", errno);

    memset(area_swap, 0, config->tamanio_swap);

    close(fd_swap);

    return EXIT_SUCCESS;
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
