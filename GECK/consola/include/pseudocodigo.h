#ifndef PSEUDOCODIGO_H_
#define PSEUDOCODIGO_H_

// shared
#include <shared.h>

// Consola
#include "consola.h" // Es para importar los enum, esto debería estar en alguna parte de las shared

// Esto va para las shared ↓↓↓↓↓↓
typedef enum {
	SET,
	ADD,
	MOV_IN,
	MOV_OUT,
	IO,
	EXIT
} t_ins;

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
	t_ins name;
	int param1;
	int param2;
} ts_ins;

typedef struct {
	op_code code;
	int size;
	void* stream;
} ts_paquete;
// Esto va para las shared ↑↑↑↑↑↑

t_list* parsear_pseudocod(char*);

t_ins procesar_inst(char* inst);
reg_cpu procesar_reg(char *registro);
dispositivos procesar_dispositivo(char *dispositivo);

ts_ins* crear_instruccion(FILE **f_pseudo);
void leer_instruccion(FILE **f_pseudo, ts_ins *inst);
void leer_parametros(FILE **f_pseudo, ts_ins *inst);

#endif /* PSEUDOCODIGO_H_ */
