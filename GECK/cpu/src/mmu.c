#include "../include/mmu.h"

extern int tam_pagina_memoria;
extern int tam_max_segmento_memoria;

extern int memoria_fd;

extern pthread_mutex_t mutex_comunicacion_memoria;

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

int recibir_marco_memoria() {
	int cod_op = recibir_operacion(memoria_fd);
	int marco;

	switch(cod_op) {
	case NUMERO_DE_MARCO_MEMORIA:
		marco = recibir_valor(memoria_fd);
		log_debug(logger_debug, "La memoria me devolvio numero de marco: %d", marco);
		break;

	case PAGE_FAULT_MEMORIA:
		marco = PAGE_FAULT_ERROR;
		log_debug(logger_debug, "La memoria me devolvio un PAGE FAULT");
		break;
	}

	return marco;
}

int pedir_marco_memoria(int pid, dir_t dir_parcial, int memoria_fd) {
	int marco;

	marco = buscar_marco_en_tlb(pid, dir_parcial.nro_seg, dir_parcial.nro_pag);

	if (marco != -1) {
		return marco;
	}

	log_debug(logger_debug, "ejecutando pedir_marco_memoria. Dir parcial: \nid_tps: %d\nnro_seg: %d\nnro_pag: %d\noffset: %d",
		dir_parcial.id_tabla_pagina, dir_parcial.nro_seg, dir_parcial.nro_pag, dir_parcial.desplazamiento_pag);

	pthread_mutex_lock(&mutex_comunicacion_memoria);
	enviar_direccion_parcial(dir_parcial, memoria_fd, ACCESO_A_MEMORIA);
	enviar_pid(memoria_fd, pid);
	marco = recibir_marco_memoria();
	pthread_mutex_unlock(&mutex_comunicacion_memoria);

	if (marco != PAGE_FAULT_ERROR) {
		ingresar_a_tlb(crear_entrada_tlb(pid, dir_parcial.nro_seg, dir_parcial.nro_pag, marco));
	}

	return marco;
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

	dir_parcial.id_tabla_pagina = seg.indice_tablas_paginas;
	dir_parcial.nro_pag = get_numero_pagina(dir_logica);
	dir_parcial.desplazamiento_pag = get_desplazamiento_pagina(dir_logica);

	return dir_parcial;
}

uint32_t leer_de_memoria(int pid, dir_t dir_parcial) {
	pthread_mutex_lock(&mutex_comunicacion_memoria);

	log_trace(logger_debug, "enviando codop LECTURA_MEMORIA");

	enviar_direccion_parcial(dir_parcial, memoria_fd, LECTURA_MEMORIA);
	enviar_pid(memoria_fd, pid);

	int cod_op = recibir_operacion(memoria_fd);

	if(cod_op == VALOR_LECTURA_MEMORIA) {
		uint32_t valor_leido = recibir_valor(memoria_fd);
		pthread_mutex_unlock(&mutex_comunicacion_memoria);

		log_debug(logger_debug, "Valor leido de memoria: %d", valor_leido);
		return valor_leido;
	}
	else {
		pthread_mutex_unlock(&mutex_comunicacion_memoria);
		log_error(logger_debug, "NO SE RECIBIO VALOR LECTURA. SE RECIBIO <%d>", cod_op);
		exit(EXIT_FAILURE);
	}
}

void escribir_en_memoria(int pid, dir_t dir_parcial, uint32_t valor) {
	pthread_mutex_lock(&mutex_comunicacion_memoria);

	enviar_direccion_parcial(dir_parcial, memoria_fd, ESCRITURA_MEMORIA);
	enviar_pid(memoria_fd, pid);
	enviar_valor(memoria_fd, valor);

	int cod_op = recibir_operacion(memoria_fd);

	if(cod_op == OK_ESCRITURA_MEMORIA) {
		log_debug(logger_debug, "Escritura de memoria ok");
	} else {
		log_error(logger_debug, "NO SE RECIBIO OK ESCRITURA. SE RECIBIO <%d>", cod_op);
	}

	pthread_mutex_unlock(&mutex_comunicacion_memoria);
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
