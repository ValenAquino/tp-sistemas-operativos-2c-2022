#include "../include/configuracion.h"

extern t_log *logger_debug;

t_configuracion_cpu* procesar_config(t_config* config) {

	int entradas_tlb = config_get_int_value(config, "ENTRADAS_TLB");
	int retardo_instruccion = config_get_int_value(config, "RETARDO_INSTRUCCION");
	char *puerto_memoria =  config_get_string_value(config, "PUERTO_MEMORIA");
	char *puerto_escucha_dispatch =  config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
	char *puerto_escucha_interrupt =  config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
	char *reemplazo_tlb = config_get_string_value(config, "REEMPLAZO_TLB");
	char *ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	char *ip_cpu = config_get_string_value(config, "IP_CPU");

	t_configuracion_cpu *datos = malloc(sizeof(t_configuracion_cpu));
	datos->entradas_tlb = (intptr_t) malloc(sizeof(int));
	datos->entradas_tlb = entradas_tlb;

	datos->retardo_instruccion = (intptr_t) malloc(sizeof(int));
	datos->retardo_instruccion = retardo_instruccion;

	datos->puerto_memoria = malloc(strlen(puerto_memoria) + 1);
	strcpy(datos->puerto_memoria, puerto_memoria);

	datos->puerto_escucha_dispatch = malloc(strlen(puerto_escucha_dispatch) + 1);
	strcpy(datos->puerto_escucha_dispatch, puerto_escucha_dispatch);

	datos->puerto_escucha_interrupt = malloc(strlen(puerto_escucha_interrupt) + 1);
	strcpy(datos->puerto_escucha_interrupt, puerto_escucha_interrupt);

	datos->reemplazo_tlb = malloc(strlen(reemplazo_tlb) + 1);
	strcpy(datos->reemplazo_tlb, reemplazo_tlb);

	datos->ip_memoria = malloc(strlen(ip_memoria) + 1);
	strcpy(datos->ip_memoria, ip_memoria);

	datos->ip_cpu = malloc(strlen(ip_cpu) + 1);
	strcpy(datos->ip_cpu, ip_cpu);

	config_destroy(config); // libero la memoria del config
	return datos;
}

void test_read_config(t_configuracion_cpu* config) {
	log_debug(logger_debug,"Leyendo de config: \n"
			"ENTRADAS_TLB: %d \n"
			"RETARDO_INSTRUCCION: %d \n"
			"PUERTO_MEMORIA: %s \n"
			"PUERTO_ESCUCHA_DISPATCH: %s \n"
			"PUERTO_ESCUCHA_INTERRUPT: %s \n"
			"REEMPLAZO_TLB: %s \n"
			"IP_MEMORIA: %s \n"
			"IP_CPU: %s \n",
			config->entradas_tlb,
			config->retardo_instruccion,
			config->puerto_memoria,
			config->puerto_escucha_dispatch,
			config->puerto_escucha_interrupt,
			config->reemplazo_tlb,
			config->ip_memoria,
			config->ip_cpu);
}
