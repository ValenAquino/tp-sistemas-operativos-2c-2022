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
	t_config* config = config_create(path);
	if (config == NULL) {
		mostrar_mensaje_para_finalizar("No se pudo abrir el archivo de configuracion en ese path\n");
		exit(EXIT_FAILURE);
	}
	return config;
}

void mostrar_mensaje_para_finalizar(char* mensaje) {
	printf("%s\n", mensaje);
	printf("Presiona Enter para finalizar.");
	getchar();
}

t_list *array_char_to_list_int(char **array) {
	t_list *lista_segmentos = list_create();

	for (int i = 0; array[i] != NULL; i++) {
		int *entero = malloc(sizeof(int));
		*entero = atoi(array[i]);

		list_add(lista_segmentos, entero);
	}

	return lista_segmentos;
}
