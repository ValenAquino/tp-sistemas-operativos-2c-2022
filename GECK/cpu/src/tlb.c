#include "../include/tlb.h"

extern pthread_mutex_t mutex_tlb;
extern t_list *tlb;
extern t_configuracion_cpu *config;
extern t_log *logger_debug;
extern t_log *logger;

void ingresar_a_tlb(entrada_tlb *entrada) {
	int cant_entradas_en_tlb = list_size(tlb);

	if (cant_entradas_en_tlb < config->entradas_tlb) {
		agregar_a_tlb(entrada);
		return;
	}

	manejar_reemplazo_para(entrada);
}

void manejar_reemplazo_para(entrada_tlb *entrada) {
	switch (get_algoritmo_reemplazo(config->reemplazo_tlb)) {
	case TLB_LRU:
		manejar_reemplazo_LRU(entrada);
		break;
	case TLB_FIFO:
		manejar_reemplazo_FIFO(entrada);
		break;
	default:
		log_error(logger_debug, "TLB - Algoritmo de reemplazo desconocido");
		break;
	}
}

void manejar_reemplazo_LRU(entrada_tlb *entrada_nueva) {
	entrada_tlb* entrada_victima = buscar_victima_lru();
	int index_victima = buscar_indice_en_tlb(entrada_victima);
	reemplazar_tlb(entrada_nueva, index_victima);
}

void manejar_reemplazo_FIFO(entrada_tlb *entrada_nueva) {
	remover_de_tlb_by_index(0);
	agregar_a_tlb(entrada_nueva);
}

void remover_de_tlb_by_index(int indice) {
	pthread_mutex_lock(&mutex_tlb);
	list_remove_and_destroy_element(tlb, indice, (void*) free);
	pthread_mutex_unlock(&mutex_tlb);
}

void agregar_a_tlb(entrada_tlb *entrada) {
	pthread_mutex_lock(&mutex_tlb);
	list_add(tlb, entrada);
	pthread_mutex_unlock(&mutex_tlb);
}

void reemplazar_tlb(entrada_tlb *entrada_nueva, int indice_victima) {
	pthread_mutex_lock(&mutex_tlb);
	list_replace(tlb, indice_victima, entrada_nueva);
	pthread_mutex_unlock(&mutex_tlb);
}

entrada_tlb* buscar_victima_lru() {
	entrada_tlb *victima = NULL;

	pthread_mutex_lock(&mutex_tlb);
	victima = list_get_minimum(tlb, (void*) comparar_ultimas_referencias); // ultima referencia mas chica = es la que hace mas tiempo que no uso
	pthread_mutex_unlock(&mutex_tlb);

	return victima;
}

void* comparar_ultimas_referencias(entrada_tlb *entrada_uno,
		entrada_tlb *entrada_dos) {
	int diff = difftime(entrada_uno->ult_ref_timestamp,
			entrada_dos->ult_ref_timestamp);
	//t1 - t2 devuelvo la de tiempo mas chica como victima.
	return diff > 0 ? entrada_dos : entrada_uno;
}

uint32_t buscar_indice_en_tlb(entrada_tlb *entrada_buscada) {
	pthread_mutex_lock(&mutex_tlb);
	for (int i = 0; i < list_size(tlb); i++) {
		entrada_tlb *entrada = list_get(tlb, i);
		if (son_entradas_iguales(entrada, entrada_buscada)) {
			pthread_mutex_unlock(&mutex_tlb);
			return i;
		}
	}
	pthread_mutex_unlock(&mutex_tlb);
	return -1;
}

int son_entradas_iguales(entrada_tlb *entrada_uno, entrada_tlb *entrada_dos) {
	return entrada_uno->frame == entrada_dos->frame
			&& entrada_uno->nro_seg == entrada_dos->nro_seg
			&& entrada_uno->nro_pag == entrada_dos->nro_pag
			&& entrada_uno->ult_ref_timestamp == entrada_dos->ult_ref_timestamp
			&& entrada_uno->pid == entrada_dos->pid;
}

entrada_tlb* crear_entrada_tlb(int pid, int nro_seg, int nro_pag, int frame) {
	entrada_tlb *entrada = (entrada_tlb*) malloc(sizeof(entrada_tlb));
	entrada->pid = pid;
	entrada->nro_seg = nro_seg;
	entrada->nro_pag = nro_pag;
	entrada->frame = frame;
	entrada->ult_ref_timestamp = time(NULL);

	return entrada;
}

algoritmo_reemplazo_tlb get_algoritmo_reemplazo(char *algoritmo) {
	if (!strcmp(algoritmo, "FIFO")) {
		return TLB_FIFO;
	} else if (!strcmp(algoritmo, "LRU")) {
		return TLB_LRU;
	}
	return -1;
}
