#include "../include/interfaz.h"

extern t_configuracion_memoria* config;
extern t_list *tablas_de_paginas;

int obtener_num_marco(dir_t direccion_parcial) {
	return PAGE_FAULT_ERROR;
}

int leer(int marco) {
	return 50;
}

void escribir(int marco, int valor) {

}

// DUDAS DE LA CREACION DE SEGMENTOS:
// -  Tenemos cantidad de entradas por tabla dada por configuracion.
// si los segmentos son de tamanio dinamico pero cada segmento tiene una sola tabla de paginas adentro
// y cada tabla de pagina tiene una cantidad paginas fija dada por configuracion y la pagina a su vez tiene un
// tamanio fijo. Entonces que habria que hacer con los tamanos de los segmentos??


t_list *crear_indices_tabla_de_paginas(t_list* tamanio_segmentos) {
	t_list * indices = list_create();

	void crear_indice(int *tamanio) {

		// Se agregan paginas vacias a la tabla de pagina.
		// el tamanio de la tabla de paginas menos 1
		// va a ser el indice que estara en el segmento.
		list_add(tablas_de_paginas, crear_paginas());
		int *indice = malloc(sizeof(int));
		*indice = list_size(tablas_de_paginas) - 1;
		list_add(indices, indice);
	}

	list_iterate(tamanio_segmentos, (void*) crear_indice);

	return indices;
}

t_list* crear_paginas() {
	t_list *paginas = list_create();

	for (int i = 0; i < config->entradas_por_tabla; i++) {
		pagina_t* pagina = get_pagina_nueva();
		list_add(paginas, pagina);
	}

	return paginas;
};

pagina_t *get_pagina_nueva() {
	pagina_t *pagina = (pagina_t*) malloc(sizeof(pagina_t));
	pagina->bit_m = 0;
	pagina->bit_p = 0;
	pagina->bit_u = 0;
	pagina->frame = 0;

	return pagina;
}
