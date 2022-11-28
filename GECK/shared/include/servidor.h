#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include "shared.h"
#include "protocolo.h"
#include "pcb.h"
#include "paquetes.h"
#include "memoria_shared.h"

int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto);
int esperar_cliente(t_log* logger, const char* name, int server_fd);

int recibir_operacion(int);
int recibir_pid(int socket_cliente);
uint32_t recibir_registro(int socket_cliente);
uint32_t recibir_respuesta_consola(int socket_cliente);
uint32_t recibir_valor(int socket_cliente);

void solicitar_pagina(int socket_conexion, t_manejar_page_fault* args);

int server_escuchar(t_log* logger, char* server_name, int server_socket, void* function_handler);

#endif /* SERVIDOR_H_ */
