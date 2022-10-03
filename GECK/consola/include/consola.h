#ifndef CONSOLA_H_
#define CONSOLA_H_

// basicas
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// sockets
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>

// commons
#include<commons/log.h>
#include<commons/config.h>

// shared
#include <shared.h>
#include <cliente.h>

typedef enum {
	SET,
	ADD,
	MOV_IN,
	MOV_OUT,
	IO,
	EXIT
} t_instr;
typedef enum {
	AX,
	BX,
	CX,
	DX
} reg_cpu;

typedef enum {
	DISCO,
	TECLADO,
	PANTALLA
} dispositivos;
typedef struct {
	t_instr instr;
	int param1;
	int param2;
} ts_instr;

t_config* procesar_config(char *, char ***, int*);
int connect_to_kernel(t_config*);
int liberar_memoria(t_log*, int, t_config*);
t_list* parsear_pseudocod(char* );

#endif /* CONSOLA_H_ */
