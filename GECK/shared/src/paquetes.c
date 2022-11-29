#include "../include/paquetes.h"


extern t_log* logger_debug;

// ENVIOS
ts_paquete* crear_paquete(op_code codigo) {
	ts_paquete* paquete = malloc(sizeof(ts_paquete));
	paquete->code = codigo;
	crear_buffer(paquete);
	return paquete;
}

void crear_buffer(ts_paquete* paquete) {
	paquete->buffer = malloc(sizeof(ts_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void agregar_a_paquete(ts_paquete* paquete, void* valor, int tamanio) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(ts_paquete* paquete, int socket_cliente) {
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void* serializar_paquete(ts_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->code), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void eliminar_paquete(ts_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void* serializar_lista_ins(t_list *lista, int size) {
	int desplazamiento = 0;

	int cant_elementos = list_size(lista);
	int size_elemento = sizeof(int);
	void* stream = malloc(size);

	memcpy(stream + desplazamiento, &cant_elementos, size_elemento);
	desplazamiento += size_elemento;
	
	for(int i = 0; i < cant_elementos; i++) {
		ts_ins *valor = list_get(lista, i);
		
		memcpy(stream + desplazamiento, &(valor->name), size_elemento);
		desplazamiento += size_elemento;

		memcpy(stream + desplazamiento, &(valor->param1), size_elemento);
		desplazamiento += size_elemento;

		memcpy(stream + desplazamiento, &(valor->param2), size_elemento);
		desplazamiento += size_elemento;
	}

	list_destroy(lista);

	return stream;
}

void* serializar_lista_tamanios_seg(t_list *lista, int size, int destruir_lista) {
	int desplazamiento = 0;

	int cant_elementos = list_size(lista);
	int size_elemento = sizeof(int);
	void* stream = malloc(size);

	memcpy(stream + desplazamiento, &cant_elementos, size_elemento);
	desplazamiento += size_elemento;
	
	for(int i = 0; i < cant_elementos; i++) {
		int *valor = list_get(lista, i);

		memcpy(stream + desplazamiento, valor, size_elemento);
		desplazamiento += size_elemento;
	}

	if(destruir_lista == 1){
		list_destroy(lista);
	}

	return stream;
}

void* serializar_lista_indices(t_list *lista, int size) {
	int desplazamiento = 0;

	int cant_elementos = list_size(lista);
	int size_elemento = sizeof(int);
	void* stream = malloc(size);

	memcpy(stream + desplazamiento, &cant_elementos, size_elemento);
	desplazamiento += size_elemento;

	for(int i = 0; i < cant_elementos; i++) {
		int *valor = list_get(lista, i);

		memcpy(stream + desplazamiento, valor, size_elemento);
		desplazamiento += size_elemento;
	}

	list_destroy(lista);

	return stream;
}

void* serializar_lista_seg(t_list *lista, int size) {
	int desplazamiento = 0;

	int cant_elementos = list_size(lista);
	int size_elemento = sizeof(int);
	void* stream = malloc(size);

	memcpy(stream + desplazamiento, &cant_elementos, size_elemento);
	desplazamiento += size_elemento;

	for(int i = 0; i < cant_elementos; i++) {
		segmento_t *segmento = list_get(lista, i);

		memcpy(stream + desplazamiento, &(segmento->tamanio_segmento), size_elemento);
		desplazamiento += size_elemento;

		memcpy(stream + desplazamiento, &(segmento->indice_tablas_paginas), size_elemento);
		desplazamiento += size_elemento;
	}

	list_destroy(lista);

	return stream;
}

void* serializar_datos_pcb(PCB *pcb, int size) {
	void *stream = malloc(size);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &(pcb->id), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(stream + desplazamiento, &(pcb->programCounter), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(stream + desplazamiento, &(pcb->socket_consola), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(stream + desplazamiento, &(pcb->estado_actual), sizeof(t_estado_proceso));
	desplazamiento += sizeof(t_estado_proceso);

	memcpy(stream + desplazamiento, pcb->registros, 4 * sizeof(uint32_t));
	desplazamiento += 4 * sizeof(uint32_t);
	
	return stream;
}

// RECEPCION

void* recibir_buffer(int* size, int socket_cliente) {
	void* buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

t_list* recibir_paquete(int socket_cliente) {
	int size;
	int tamanio;
	void* buffer;
	
	int desplazamiento = 0;	
	t_list* valores = list_create();

	buffer = recibir_buffer(&size, socket_cliente);

	while(desplazamiento < size) {
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);

		void* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		
		list_add(valores, valor);
	}

	free(buffer);
	return valores;
}

t_list* deserializar_lista_inst(void *stream) {
	int cant_elementos;
	int desplazamiento = 0;
	t_list* lista = list_create();

	memcpy(&cant_elementos, stream + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);
	
	for(int i = 0; i < cant_elementos; i++) {
		ts_ins *valor = malloc(sizeof(ts_ins));
		
		memcpy(&valor->name, stream + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		memcpy(&valor->param1, stream + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		memcpy(&valor->param2, stream + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		list_add(lista, valor);
	}

	return lista;
}

t_list* deserializar_lista_segm(void *stream) {
	int cant_elementos;
	int desplazamiento = 0;
	int size_elemento = sizeof(int);
	t_list* lista = list_create();

	memcpy(&cant_elementos, stream + desplazamiento, size_elemento);
	desplazamiento += size_elemento;
	
	for(int i = 0; i < cant_elementos; i++) {
		segmento_t *segmento = malloc(sizeof(segmento_t));

		memcpy(&segmento->tamanio_segmento, stream + desplazamiento, size_elemento);
		desplazamiento += size_elemento;

		memcpy(&segmento->indice_tablas_paginas, stream + desplazamiento, size_elemento);
		desplazamiento += size_elemento;

		list_add(lista, segmento);
	}

	return lista;
}


t_list* deserializar_lista_indices(void *stream) {
	int cant_elementos;
	int desplazamiento = 0;
	int size_elemento = sizeof(int);
	t_list* lista = list_create();

	memcpy(&cant_elementos, stream + desplazamiento, size_elemento);
	desplazamiento += size_elemento;

	for(int i = 0; i < cant_elementos; i++) {
		int *valor = malloc(sizeof(int));
		memcpy(valor, stream + desplazamiento, size_elemento);
		desplazamiento += size_elemento;
		list_add(lista, valor);
	}

	return lista;
}

t_list* deserializar_lista_tamanios_segm(void *stream) {
	int cant_elementos;
	int desplazamiento = 0;
	int size_elemento = sizeof(int);
	t_list* lista = list_create();

	memcpy(&cant_elementos, stream + desplazamiento, size_elemento);
	desplazamiento += size_elemento;

	for(int i = 0; i < cant_elementos; i++) {
		int *valor = malloc(sizeof(int));
		memcpy(valor, stream + desplazamiento, size_elemento);
		desplazamiento += size_elemento;
		list_add(lista, valor);
	}

	return lista;
}

PCB* deserializar_pcb(void* data, void* inst, void* segm, void* tamanios_segmentos) {
	PCB* pcb = (PCB*) malloc(sizeof(PCB));
	int desplazamiento = 0;

	memcpy(&(pcb->id), data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&(pcb->programCounter), data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&(pcb->socket_consola), data + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&(pcb->estado_actual), data + desplazamiento, sizeof(t_estado_proceso));
	desplazamiento += sizeof(t_estado_proceso);

	memcpy(pcb->registros, data + desplazamiento, 4 * sizeof(uint32_t));
	desplazamiento += 4 * sizeof(uint32_t);

	pcb->instrucciones = deserializar_lista_inst(inst);
	pcb->tablaSegmentos = deserializar_lista_segm(segm);
	pcb->tamanios_segmentos = deserializar_lista_tamanios_segm(tamanios_segmentos);

	free(data);
	free(inst);
	free(segm);
	free(tamanios_segmentos);

	return pcb;
}

void enviar_pcb(PCB* pcb, int socket_fd, op_code op_code) {
	ts_paquete* paquete = crear_paquete(op_code);

	// 3 ints de PC, ID, SOCKET + 4 uint32_t de los registros + 1 t_estado_proceso
	int size_data = sizeof(int) * 3 + sizeof(uint32_t) * 4 + sizeof(t_estado_proceso);
	int size_ins = sizeof(ts_ins) * list_size(pcb->instrucciones) + sizeof(int);
	int size_tamanios_seg = sizeof(int) * list_size(pcb->tamanios_segmentos) + sizeof(int);
	int size_seg = sizeof(segmento_t) * list_size(pcb->tablaSegmentos) + sizeof(int);

	void* data = serializar_datos_pcb(pcb, size_data);
	void* inst = serializar_lista_ins(pcb->instrucciones, size_ins);
	void* tam_segm = serializar_lista_tamanios_seg(pcb->tamanios_segmentos, size_tamanios_seg, 1);
	void* segm = serializar_lista_seg(pcb->tablaSegmentos, size_seg);

	agregar_a_paquete(paquete, data, size_data);
	agregar_a_paquete(paquete, inst, size_ins);
	agregar_a_paquete(paquete, tam_segm, size_tamanios_seg);
	agregar_a_paquete(paquete, segm, size_seg);

	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);
}

PCB* recibir_pcb(int cliente_socket) {
	t_list *listas = recibir_paquete(cliente_socket);

	void* datos = list_get(listas, 0);
	void* inst  = list_get(listas, 1);
	void* tam_segm  = list_get(listas, 2);
	void* segm  = list_get(listas, 3);
	list_destroy(listas);

	PCB* pcb = deserializar_pcb(datos, inst, segm, tam_segm);

	return pcb;
}

t_list* deserializar_lista_tiempos(void* stream) {
	t_list* lista = list_create();
	int desplazamiento = 0;

	for (int i = 0; i < 2; i++) {
		int *tiempo = malloc(2*sizeof(int));
		
		memcpy(&(tiempo[0]), stream + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);	

		memcpy(&(tiempo[1]), stream + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		list_add(lista, tiempo);
	}

	return lista;
}

void enviar_solicitud_crear_estructuras_memoria(t_list *tamanios_segmentos, int socket_fd, int pid) {
	ts_paquete* paquete = crear_paquete(CREAR_ESTRUCTURAS_MEMORIA);
	t_list *tamanios_segmentos_copy = list_duplicate(tamanios_segmentos);
	int size_tamanios_seg = sizeof(int) * list_size(tamanios_segmentos_copy) + sizeof(int);

	void* tam_segm = serializar_lista_tamanios_seg(tamanios_segmentos_copy, size_tamanios_seg, 0);
	agregar_a_paquete(paquete, tam_segm, size_tamanios_seg);

	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);

	enviar_pid(socket_fd, pid);
}

t_list* recibir_solicitud_crear_estructuras_memoria(int cliente_socket) {
	t_list *listas = recibir_paquete(cliente_socket);

	log_debug(logger_debug, "recibir_solicitud_crear_estructuras_memoria list paquete size: %d", list_size(listas));

	void* tam_segm = list_get(listas, 0);

	list_destroy(listas);

	t_list* tamanios_segmentos = deserializar_lista_tamanios_segm(tam_segm);

	return tamanios_segmentos;
}

void enviar_indices_tablas_de_paginas(t_list *indices, int socket_fd, int pid) {
	ts_paquete* paquete = crear_paquete(RESPUESTA_INDICES_T_PS);

	int size_tamanios_indices = sizeof(int) * list_size(indices) + sizeof(int);
	log_debug(logger_debug, "enviar_indices_tablas_de_paginas list paquete size: %d", list_size(indices));

	void* tam_indices = serializar_lista_indices(indices, size_tamanios_indices);
	agregar_a_paquete(paquete, tam_indices, size_tamanios_indices);

	enviar_paquete(paquete, socket_fd);
	eliminar_paquete(paquete);

	enviar_pid(socket_fd,pid);
}

t_list* recibir_indices_tablas_de_paginas(int cliente_socket) {
	t_list *listas = recibir_paquete(cliente_socket);

	log_debug(logger_debug, "recibir_indices_tablas_de_paginas list paquete size: %d", list_size(listas));

	void* tam_indices = list_get(listas, 0);

	list_destroy(listas);

	t_list* tamanios_segmentos = deserializar_lista_indices(tam_indices);

	return tamanios_segmentos;
}

void* serializar_direccion_parcial(dir_t direccion, int size) {
	void *stream = malloc(size);
	int desplazamiento = 0;

	memcpy(stream + desplazamiento, &(direccion.nro_seg), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(stream + desplazamiento,&(direccion.nro_pag), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(stream + desplazamiento, &(direccion.desplazamiento_pag), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(stream + desplazamiento, &(direccion.id_tabla_pagina),  sizeof(int));
	desplazamiento += sizeof(int);

	return stream;
}

dir_t deserializar_direccion_parcial(void* stream) {
	dir_t* direccion = (dir_t*) malloc(sizeof(dir_t));

	int desplazamiento = 0;

	memcpy(&(direccion->nro_seg), stream + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&(direccion->nro_pag), stream + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&(direccion->desplazamiento_pag), stream + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(&(direccion->id_tabla_pagina), stream + desplazamiento, sizeof(int));
	desplazamiento += sizeof(int);

	dir_t resultado;
	resultado.nro_seg = direccion->nro_seg;
	resultado.nro_pag = direccion->nro_pag;
	resultado.desplazamiento_pag = direccion->desplazamiento_pag;
	resultado.id_tabla_pagina = direccion->id_tabla_pagina;

	free(stream);
	free(direccion);

	return resultado;
}

void enviar_direccion_parcial(dir_t direccion, int cliente_socket, int codop) {
	ts_paquete* paquete = crear_paquete(codop);

	int size_dir = sizeof(dir_t);

	void* dir_stream = serializar_direccion_parcial(direccion, size_dir);

	agregar_a_paquete(paquete, dir_stream, size_dir);

	enviar_paquete(paquete, cliente_socket);

	eliminar_paquete(paquete);
	free(dir_stream);
}

dir_t recibir_direccion_parcial(int cliente_socket) {
	t_list *listas = recibir_paquete(cliente_socket);

	log_debug(logger_debug, "recibir_direccion_parcial list paquete size: %d", list_size(listas));

	void* direccion = list_get(listas, 0);

	dir_t direccion_deserializada = deserializar_direccion_parcial(direccion);

	list_destroy(listas);
	return direccion_deserializada;
}



