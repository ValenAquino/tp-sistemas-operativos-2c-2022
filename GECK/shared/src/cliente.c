#include "../include/cliente.h"

extern t_log* logger;

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(ip, puerto, &hints, &server_info) != 0) {
		log_error(logger, "get adress info error");
		return -1;
	}

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(
		server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol
	);

	if(socket_cliente == -1) {
		log_error(logger, "socket error");
		return -1;
	}

	// Ahora que tenemos el socket, vamos a conectarlo
	int conexion = connect(
		socket_cliente, server_info->ai_addr, server_info->ai_addrlen
	);

	if(conexion == -1) {
		log_error(logger, "connect error");
		return -1;
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_codop(int socket_conexion, int cod_op) {
	void * a_enviar = malloc(sizeof(int));

	memcpy(a_enviar, &(cod_op), sizeof(int));
	send(socket_conexion, a_enviar, sizeof(int), 0);
}

