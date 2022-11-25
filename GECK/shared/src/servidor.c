#include "../include/servidor.h"

// TODO comprobar que las syscall devuelvan >= 0 sino tirar error
int iniciar_servidor(t_log* logger, const char* server_name, char* ip, char* puerto) {
	int socket_servidor;

	struct addrinfo condiciones, *servinfo;

	// pone 0 a los sizeof(hints) caracteres de hints
	memset(&condiciones, 0, sizeof(condiciones));

	condiciones.ai_family = AF_UNSPEC; // No importa si IPv4 o IPv6
	condiciones.ai_socktype = SOCK_STREAM;
	condiciones.ai_flags = AI_PASSIVE; // Flag para que use el IP de este dispositivo

	// Como está definido un IP no se tiene en cuenta la flag passive
	getaddrinfo(ip, puerto, &condiciones, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(
			servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol
	);

	// si se interrumpe la ejecucion no tengo que esperar 2min para reutilizar ese socket
	// en ese ip y puerto
	int activado = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	log_trace(logger, "%s escuchando en la IP: %s PUERTO: %s", server_name, ip, puerto);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN); // hay que cambiar SOMAXCONN por el grado de multiprogramacion

	freeaddrinfo(servinfo);
	log_trace(logger, "%s listo para recibir clientes", server_name);

	return socket_servidor;
}

// quizas haya que modificarla para usar hilos
int esperar_cliente(t_log* logger, const char* name, int server_fd) {
	int cliente_fd = accept(server_fd, NULL, NULL);

	log_info(logger, "Se ha conectado un cliente a %s", name);

	return cliente_fd;
}

int recibir_operacion(int socket_cliente) {
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

int recibir_pid(int socket_cliente) {
	return recibir_operacion(socket_cliente);
}


uint32_t recibir_registro(int socket_cliente) {
	uint32_t cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(uint32_t), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

uint32_t recibir_respuesta_consola(int socket_cliente) {
	return recibir_registro(socket_cliente);
}

uint32_t recibir_valor(int socket_cliente) {
	return recibir_registro(socket_cliente);
}


int server_escuchar(t_log* logger, char* server_name, int server_socket, void* function_handler) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_manejar_conexion_args* args = malloc(sizeof(t_manejar_conexion_args));
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) function_handler, (void*) args);
        pthread_detach(hilo);
        return 1;
    }

    return 0;
}

void solicitar_pagina(PCB* pcb, int socket_conexion, int num_segmento, int num_pagina) {
	enviar_codop(socket_conexion, PAGINA_SOLICITADA);
	enviar_valor(socket_conexion, pcb->id);
	enviar_valor(socket_conexion, num_segmento);
	enviar_valor(socket_conexion, num_pagina);
}

