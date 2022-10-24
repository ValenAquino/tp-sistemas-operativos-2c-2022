

#include "../include/planificadorLargoPlazo.h"


extern t_log* logger;
extern t_configuracion_kernel *config;

extern t_list* procesosNew;
extern t_list* procesosReady;

extern int cpu_dispatch_fd;

void nuevoProceso(PCB* pcb) {
	list_add(procesosNew, pcb);
	log_info(logger, "Se agrego un proceso de id: %d a la cola de NEW", pcb->id);

	pasarAReady();
}

void imprimir_ready() {
	int size = list_size(procesosReady);
	char *pids = string_new();

	for (int i = 0; i < size-1; i++) {
		PCB *pcb = list_get(procesosReady, i);
		string_append(&pids, string_itoa(pcb->id));
		string_append(&pids, ", ");
	}

	PCB *pcb = list_get(procesosReady, size-1);
	string_append(&pids, string_itoa(pcb->id));

	log_info(logger, "lista ready [%s]", pids);
	free(pids);
}

void pasarAReady() {
	log_trace(logger, "procesos ready: %d, grado multi: %d", list_size(procesosReady), config->grado_max_multiprogramacion);

	if(list_size(procesosReady) < config->grado_max_multiprogramacion) {
		PCB* pcb = list_remove(procesosNew, 0);
		list_add(procesosReady, pcb);
		log_info(logger, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", pcb->id);
		imprimir_ready();
	}
}

/*
	int id;
    int programCounter;
    uint32_t registros[4];
    t_list* instrucciones;
    t_list* tablaSegmentos; //lo resuelve memoria a peticion del kernel
*/

void* serializar_pcb(PCB* pcb) {
	int size_ins = sizeof(ts_ins) * list_size(pcb->instrucciones) + sizeof(int);
	int size_seg = sizeof(int) * list_size(pcb->tablaSegmentos) + sizeof(int);
	int size_datos = 2 * sizeof(int) + 4 * sizeof(uint32_t);
	
	void* lista_ins = serializar_lista_ins(pcb->instrucciones, size_ins);
	void* lista_seg = serializar_lista_seg(pcb->instrucciones, size_seg);

	void* stream = malloc(size_datos + size_ins + size_datos);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &pcb->id, sizeof(int));
	desplazamiento += sizeof(int);
	
	memcpy(stream + desplazamiento, &pcb->programCounter, sizeof(int));
	desplazamiento += sizeof(int);

	for(int i = 0; i<4; i++) {
		memcpy(stream + desplazamiento, &pcb->registros[i], sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
	}

	// usar la función que dejé en consola
	
	// TAMAÑO DE LISTA INSTRUCCIONES
	memcpy(stream + desplazamiento, size_ins, sizeof(int));
	desplazamiento += sizeof(int);

	// LISTA DE INSTRUCCIONES
	memcpy(stream + desplazamiento, lista_ins, size_ins);
	desplazamiento += size_ins;
	
	// LISTA DE SEGMENTOS
	memcpy(stream + desplazamiento, lista_seg, size_seg);
	desplazamiento += size_seg;

	free(pcb);
	free(lista_ins);
	free(lista_seg);

	return stream;
}

PCB* deserializar_pcb(void *stream) {
	int size_ins;
	int size_seg;
	PCB* pcb = malloc(sizeof(PCB));
	int desplazamiento = 0;
		
	memcpy(&pcb->id, stream + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&pcb->programCounter, stream + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	
	for(int i = 0; i<4; i++) {
		memcpy(&pcb->registros[i], stream + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);
	}

	pcb->instrucciones = deserializar_lista_inst(stream+desplazamiento);
	pcb->tablaSegmentos = deserializar_lista_segm(stream+desplazamiento);

	return pcb;
}

void dispatch_pcb(PCB *pcb) {
	ts_paquete* paquete = crear_paquete(DISPATCH_PCB);

}

/*
void finalizarProceso(int processId) {
	PCB* proceso = list_find(procesosNew, )
	list_remove(procesosNew, proceso );
}
*/
