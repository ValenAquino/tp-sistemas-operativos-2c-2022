#include "../include/interfaz.h"

extern t_configuracion_memoria *config;
extern t_list *tablas_de_paginas;

extern pthread_mutex_t tablas_de_paginas_mutex;

pagina_t* obtener_pagina(int id_en_tablas_de_ps, int nro_pag) {
	log_debug(logger_debug, "Buscando pagina: %d, en el indice: %d", nro_pag, id_en_tablas_de_ps);
	pthread_mutex_lock(&tablas_de_paginas_mutex);
	t_list *tabla_paginas = list_get(tablas_de_paginas, id_en_tablas_de_ps);
	pthread_mutex_unlock(&tablas_de_paginas_mutex);

	pagina_t *pagina = list_get(tabla_paginas, nro_pag);
	log_debug(logger_debug, "Se encontro una pagina del pid: %d", pagina->pid);

	return pagina;
}

int obtener_num_marco(dir_t direccion_parcial, int pid) {
	pagina_t *pagina = obtener_pagina(direccion_parcial.id_tabla_pagina,
			direccion_parcial.nro_pag);

	log_info(logger, "Acceso a Tabla de Páginas: PID: <%d> - Página: <%d> - Marco: <%d>",
				pid,direccion_parcial.nro_pag, pagina->frame);

	if (pagina->bit_p == 1) {
		pagina->bit_u = 1;
		return pagina->frame;
	}

	return PAGE_FAULT_ERROR;
}

uint32_t leer(int pid, dir_t dir_parcial) {
	pagina_t *pagina = obtener_pagina(dir_parcial.id_tabla_pagina,
			dir_parcial.nro_pag);
	log_debug(logger_debug, "leer() - PID: %d - PID leido de pagina: %d", pid, pagina->pid);
	return leer_de_memoria_principal(pagina, dir_parcial.desplazamiento_pag);
}

void escribir(int pid, dir_t dir_parcial, int valor) {
	pagina_t *pagina = obtener_pagina(dir_parcial.id_tabla_pagina,
			dir_parcial.nro_pag);
	log_debug(logger_debug, "escribir() - PID: %d - PID leido de pagina: %d", pid, pagina->pid);
	escribir_en_memoria_principal(pagina, dir_parcial.desplazamiento_pag, valor);
}

// DUDAS DE LA CREACION DE SEGMENTOS:
// -  Tenemos cantidad de entradas por tabla dada por configuracion.
// si los segmentos son de tamanio dinamico pero cada segmento tiene una sola tabla de paginas adentro
// y cada tabla de pagina tiene una cantidad paginas fija dada por configuracion y la pagina a su vez tiene un
// tamanio fijo. Entonces que habria que hacer con los tamanos de los segmentos??

t_list* crear_indices_tabla_de_paginas(t_list *tamanio_segmentos, int pid) {
	t_list *indices = list_create();

	int iterator_index = 0;
	void crear_indice(int *tamanio) {

		// Se agregan paginas vacias a la tabla de pagina.
		// el tamanio de la tabla de paginas menos 1
		// va a ser el indice que estara en el segmento.
		list_add(tablas_de_paginas, crear_paginas(pid));
		int *indice = malloc(sizeof(int));
		*indice = list_size(tablas_de_paginas) - 1;
		list_add(indices, indice);
		log_info(logger,
					"PID: <%d> - Segmento: <%d> - TAMAÑO: <%d> paginas",
					pid, iterator_index, (int) ceil(*tamanio / config->tam_pagina));
		iterator_index++;
	}

	list_iterate(tamanio_segmentos, (void*) crear_indice);

	return indices;
}

t_list* crear_paginas(int pid) {
	t_list *paginas = list_create();

	for (int i = 0; i < config->entradas_por_tabla; i++) {
		pagina_t *pagina = crear_pagina_vacia(pid);
		void* data_pagina = malloc(config->tam_pagina);
		memset(data_pagina, 0, config->tam_pagina);
		// Los parametros tres y cuatro no importan ya que solo se usan si el ultimo parametro es 1
		cargar_en_swap_pagina_entera(pagina, data_pagina, 0, 0, 0);
		list_add(paginas, pagina);
	}

	return paginas;
}

pagina_t* crear_pagina_vacia(int pid) {
	pagina_t *pagina = (pagina_t*) malloc(sizeof(pagina_t));
	pagina->bit_m = 0;
	pagina->bit_p = 0;
	pagina->bit_u = 0;
	pagina->frame = 0;
	pagina->pos_swap = 0;
	pagina->pid = pid;

	return pagina;
}


void ejecutar_retardo_memoria(char* motivo_retardo) {
	log_debug(logger_debug,	"Ejecutando retardo memoria de %d milisegundos por %s",
			config->retardo_memoria, motivo_retardo);
	usleep(config->retardo_memoria * 1000);
}
