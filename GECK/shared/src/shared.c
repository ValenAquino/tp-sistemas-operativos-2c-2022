#include "../include/shared.h"

void enviar_codop(int socket_conexion, int cod_op) {
	void * a_enviar = malloc(sizeof(int));

	memcpy(a_enviar, &(cod_op), sizeof(int));
	send(socket_conexion, a_enviar, sizeof(int), 0);
}

void enviar_pid(int socket_conexion, int pid) {
	enviar_codop(socket_conexion, pid);
}

void enviar_registro(int socket_conexion, uint32_t reg) {
	void * a_enviar = malloc(sizeof(uint32_t));

	memcpy(a_enviar, &(reg), sizeof(uint32_t));
	send(socket_conexion, a_enviar, sizeof(uint32_t), 0);
}

void enviar_valor(int socket_conexion, uint32_t reg) {
    enviar_registro(socket_conexion, reg);
}

void liberar_conexion(int socket_cliente) {
    enviar_codop(socket_cliente, DESCONEXION_CONTROLADA);
	close(socket_cliente);
}

t_config* abrir_configuracion(char* path) {
	t_config* config;

	config = config_create(path);

	return config;
}
