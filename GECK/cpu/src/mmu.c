#include "./include/mmu.h"

extern int entradas_por_tabla;
extern int tam_pagina;

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
	enviar_direccion(dir_parcial, memoria_fd);

	sem_wait(&sem_acceso_memoria);

	if (MARCO_MEMORIA == -1) {
		return EXIT_FAILURE; // Page Fault
	} else {
		log_trace(logger_debug, "Voy a leer la memoria en el marco: %d", MARCO_MEMORIA);
		leer_de_memoria(MARCO_MEMORIA, dir_parcial.desplazamiento_pag);
		return EXIT_SUCCESS;
	}

//	enviar_valor(memoria_fd, direccion);
}

dir_t traducir_direccion(int dir_logica, t_list* tabla_segmentos) {
	dir_t dir_parcial;

	dir_parcial.nro_seg = get_nro_seg(dir_logica);
	segmento_t seg = list_get(tabla_segmentos, dir_parcial.nro_seg);
	int desplazamiento_seg = get_desplazamiento_seg(dir_logica);

	int seg_fault = checkear_seg_fault(desplazamiento_seg, seg);

	if(seg_fault) {
		dir_parcial.nro_pag = -1;
		return dir_parcial;
	}

	dir_parcial.id_tabla_pagina = get_id_tabla_pagina(dir_logica);
	dir_parcial.nro_pag = get_nro_pag(dir_logica);
	dir_parcial.desplazamiento_pag = get_desplazamiento_pag(dir_logica);

	return dir_parcial;
}
