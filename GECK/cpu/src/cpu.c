#include "../include/cpu.h"

t_log *logger;
t_log *logger_debug;

t_configuracion_cpu *config;

int FLAG_FIN_QUANTUM;
int FLAG_PAGE_FAULT;

int entradas_por_tabla_memoria;
int tam_pagina_memoria;
int tam_max_segmento_memoria;

t_list *tiempos_io;
t_list *tlb;

uint32_t REG_AX;
uint32_t REG_BX;
uint32_t REG_CX;
uint32_t REG_DX;

pthread_mutex_t mutex_tlb;
pthread_mutex_t mutex_comunicacion_memoria;
pthread_mutex_t mutex_flag_quantum;

int memoria_fd;
char *config_path;

int main(int argc, char **argv) {
    signal(SIGINT, sighandler);

	if (argc < 2) {
		config_path = "cpu.config";
	} else {
		config_path = argv[1];
	}

	iniciar_cpu();
	log_debug(logger_debug, "MAIN CPU");
	inciar_comunicacion_con_memoria();

	iniciar_servidores_de_escucha();

	terminar_cpu();
	return EXIT_SUCCESS;
}

void iniciar_cpu() {
	t_config *config_file = abrir_configuracion(config_path);
	crear_loggers("cpu", &logger, &logger_debug, config_file);
	config = procesar_config(config_file);

	log_debug(logger_debug, "config: %s", config_path);
	test_read_config(config);

	tlb = list_create();

	pthread_mutex_init(&mutex_tlb, NULL);
	pthread_mutex_init(&mutex_comunicacion_memoria, NULL);
	pthread_mutex_init(&mutex_flag_quantum, NULL);
}

void iniciar_servidores_de_escucha() {
	int server_interrupt_fd =
		crear_servidor(config->ip_cpu,config->puerto_escucha_interrupt, INTERRUPT_SERVER_NAME);
	int server_dispatch_fd =
		crear_servidor(config->ip_cpu, config->puerto_escucha_dispatch, DISPATCH_SERVER_NAME);

	while(
		server_escuchar_interrupt(INTERRUPT_SERVER_NAME, server_interrupt_fd) &&
		server_escuchar_dispatch(DISPATCH_SERVER_NAME,server_dispatch_fd)
	);
}

int crear_servidor(char *ip, char *puerto, char *server_name) {
	int server_fd = iniciar_servidor(logger, server_name, ip, puerto);

	log_info(logger_debug, "%s listo para recibir al cliente", server_name);

	return server_fd;
}

void sighandler(int x) {
	switch (x) {
	case SIGINT:
		terminar_cpu();
		exit(EXIT_SUCCESS);
		break;
	}
}

void destruir_loggers() {
	log_destroy(logger);
	log_destroy(logger_debug);
}

void destruir_listas_y_elementos() {
	list_destroy_and_destroy_elements(tlb, free);
}


void liberar_conexiones() {
	liberar_conexion(memoria_fd);
}

void terminar_cpu() {
	liberar_configuracion_cpu(config);
	destruir_listas_y_elementos();
	liberar_conexiones();
	destruir_loggers();
	limpiar_tlb();
}
