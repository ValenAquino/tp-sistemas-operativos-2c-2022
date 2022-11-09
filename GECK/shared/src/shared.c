#include "../include/shared.h"

void enviar_codop(int socket_conexion, int cod_op) {
	void * a_enviar = malloc(sizeof(int));

	memcpy(a_enviar, &(cod_op), sizeof(int));
	send(socket_conexion, a_enviar, sizeof(int), 0);
}


void enviar_registro(int socket_conexion, uint32_t reg) {
	void * a_enviar = malloc(sizeof(uint32_t));

	memcpy(a_enviar, &(reg), sizeof(uint32_t));
	send(socket_conexion, a_enviar, sizeof(uint32_t), 0);
}

void liberar_conexion(int socket_cliente) {
    enviar_codop(socket_cliente, DESCONEXION_CONTROLADA);
	close(socket_cliente);
}

bool config_has_all_properties(t_config* cfg, char** properties) {
    for(uint8_t i = 0; properties[i] != NULL; i++) {
        if(!config_has_property(cfg, properties[i]))
            return false;
    }

    return true;
}

char* leer_archivo_completo(char* path) {
    FILE* fp = fopen(path, "r+");

    if(fp == NULL)
        return NULL;

    fseek(fp, 0L, SEEK_END);
    long lSize = ftell(fp);
    rewind(fp);

    char* text = calloc(1, lSize + 1);
    fread(text, lSize, 1, fp);
    fclose(fp);

    return text;
}

t_config* abrir_configuracion(char* path)
{
	t_config* config;

	config = config_create(path);

	return config;
}
