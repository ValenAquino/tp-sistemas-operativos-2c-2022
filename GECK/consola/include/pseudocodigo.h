#ifndef PSEUDOCODIGO_H_
#define PSEUDOCODIGO_H_

// shared
#include <shared.h>
#include <sharedLocal.h>

t_list* parsear_pseudocod(char*);

t_ins procesar_inst(char* inst);
reg_cpu procesar_reg(char *registro);
dispositivos procesar_dispositivo(char *dispositivo);

ts_ins* crear_instruccion(FILE **f_pseudo);
void leer_instruccion(FILE **f_pseudo, ts_ins *inst);
void leer_parametros(FILE **f_pseudo, ts_ins *inst);

#endif /* PSEUDOCODIGO_H_ */
