#include "../include/mmu.h"

extern int tam_pagina_memoria;
extern int tam_max_segmento_memoria;

extern int MARCO_MEMORIA;
extern int memoria_fd;

extern sem_t sem_acceso_memoria;

/*
 * Agarrar dir log

 * calcular nr seg
 * calcular desplazmiento dentro del segmento
 * verificar seg fault
 * hallar indice de tp en memoria
 * calcular pagina
 * calcular desplazamiento_pagina
 * se devuelve: indice tabla de paginas, nro pagina y desplazamiento_pagina

 *
 *
 */

int pedir_marco_memoria(dir_t dir_parcial, int memoria_fd) {
	enviar_direccion_parcial(dir_parcial, memoria_fd);

	sem_wait(&sem_acceso_memoria);

	return MARCO_MEMORIA;

}

dir_t traducir_direccion(int dir_logica, t_list *tabla_segmentos) {
	dir_t dir_parcial;

	int nro_seg = get_numero_segmento(dir_logica);
	dir_parcial.nro_seg = nro_seg;
	segmento_t seg = *((segmento_t*) list_get(tabla_segmentos, nro_seg));
	int desplazamiento_seg = get_desplazamiento_segmento(dir_logica);

	int seg_fault = hay_seg_fault(desplazamiento_seg, seg.tamanio_segmento);

	if (seg_fault) {
		dir_parcial.nro_seg = SEG_FAULT_ERROR;
		return dir_parcial;
	}

	dir_parcial.id_tabla_pagina = seg.num_pagina;
	dir_parcial.nro_pag = get_numero_pagina(dir_logica);
	dir_parcial.desplazamiento_pag = get_desplazamiento_pagina(dir_logica);

	return dir_parcial;
}


void leer_de_memoria(int marco, int offset, int memoria_fd) {
	enviar_codop(memoria_fd, LECTURA_MEMORIA);
	enviar_valor(memoria_fd, marco);
	enviar_valor(memoria_fd, offset);
}

void escribir_en_memoria(int marco, int valor) {
	enviar_codop(memoria_fd, ESCRITURA_MEMORIA);
	enviar_valor(memoria_fd, marco);
	enviar_valor(memoria_fd, valor);
}

// CALCULOS

int get_numero_segmento(int dir_logica) {
	return floor(dir_logica / tam_max_segmento_memoria);
}

int get_desplazamiento_segmento(int dir_logica) {
	return dir_logica % tam_max_segmento_memoria;
}

int get_numero_pagina(int desplazamiento_segmento) {
	return floor(desplazamiento_segmento / tam_pagina_memoria);
}

int get_desplazamiento_pagina(int desplazamiento_segmento) {
	return desplazamiento_segmento % tam_pagina_memoria;
}

int hay_seg_fault(int desplazamiento_segmento, int tamanio_segmento) {
	return desplazamiento_segmento > tamanio_segmento;
}
