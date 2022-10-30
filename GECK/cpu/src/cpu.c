
#include "../include/cpu.h"

t_log* logger;
t_configuracion_cpu *config;

uint32_t REG_AX;
uint32_t REG_BX;
uint32_t REG_CX;
uint32_t REG_DX;

int main() {
	iniciar_cpu();

	int server_interrupt_fd = crear_servidor(config->ip_cpu, config->puerto_escucha_interrupt, INTERRUPT_SERVER_NAME);
	int server_dispatch_fd = crear_servidor(config->ip_cpu, config->puerto_escucha_dispatch, DISPATCH_SERVER_NAME);

	while (server_escuchar(INTERRUPT_SERVER_NAME, server_interrupt_fd) &&
			server_escuchar(DISPATCH_SERVER_NAME, server_dispatch_fd));

	return EXIT_SUCCESS;
}

void iniciar_cpu() {
	logger = log_create("cpu.log", "CPU", 1, LOG_LEVEL_DEBUG);
	config = procesar_config("cpu.config");
	test_read_config(config);
}

int crear_servidor(char* ip, char* puerto, char* server_name) {
	int server_fd = iniciar_servidor(logger, server_name, ip, puerto);

	log_info(logger, "%s listo para recibir al cliente", server_name);

	return server_fd;
}

