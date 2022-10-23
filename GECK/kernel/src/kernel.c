#include "../include/kernel.h"

t_log* logger;
t_list* procesosNew;
t_list* procesosReady;
t_configuracion_kernel *config;
	
int main() {
	inicializar_kernel();

	int server_fd = iniciar_servidor_kernel(config->ip_kernel, config->puerto_kernel);

	int cpu_interrupt_fd = conectar_con("CPU (interrupt)", config->ip_cpu, config->puerto_cpu_interrupt);
	int cpu_dispatch_fd = conectar_con("CPU (dispatch)", config->ip_cpu, config->puerto_cpu_dispatch);
	int memoria_fd = conectar_con("Memoria", config->ip_memoria, config->puerto_memoria);

	send_debug(cpu_interrupt_fd);
	send_debug(cpu_dispatch_fd);
	send_debug(memoria_fd);

	while (server_escuchar(SERVERNAME, server_fd));
	return EXIT_SUCCESS;
}

int iniciar_servidor_kernel(char* ip, char* puerto) {
	int server_fd = iniciar_servidor(logger, SERVERNAME, ip, puerto);

	log_info(logger, "Kernel listo para recibir al cliente");

	return server_fd;
}

void inicializar_kernel() {
	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	config = procesar_config("kernel.config");
	procesosNew = list_create();
	procesosReady = list_create();
}

t_configuracion_kernel* procesar_config(char *config_path) {
	char **listaDispositivos;
	char **listaTiempos;
	t_config* nuevo_config = config_create(config_path);

	    if (nuevo_config== NULL) {
	    		log_error(logger, "No se pudo abrir el archivo de configuracion en ese path");
	    		exit(EXIT_FAILURE);
	    	}
	    char *ip_kernel = config_get_string_value(nuevo_config, "IP_KERNEL");                           // leo ip de kernel
	    char *ip_memoria = config_get_string_value(nuevo_config, "IP_MEMORIA");                           // leo ip de memoria
	    char *puerto_memoria = config_get_string_value(nuevo_config, "PUERTO_MEMORIA");                   // leo puerto
	    char *ip_cpu = config_get_string_value(nuevo_config, "IP_CPU");                                   // leo ip de cpu
	    char *puerto_kernel = config_get_string_value(nuevo_config, "PUERTO_KERNEL");                           // leo ip de kernel
	    char *puerto_cpu_dispatch = config_get_string_value(nuevo_config, "PUERTO_CPU_DISPATCH");         // leo puerto de cpu dispatch
	    char *puerto_cpu_interrupt = config_get_string_value(nuevo_config, "PUERTO_CPU_INTERRUPT");       // leo puerto de cpu interrupt
	    char *puerto_escucha = config_get_string_value(nuevo_config, "PUERTO_ESCUCHA");                   // leo puerto de escucha
	    char *algoritmo_planificacion = config_get_string_value(nuevo_config, "ALGORITMO_PLANIFICACION"); // leo algoritmo de planificacion
	    int grado_max_multiprogramacion = config_get_int_value(nuevo_config, "GRADO_MAX_MULTIPROGRAMACION"); // leo grado multiprogramacion
	    int quantum_rr = config_get_int_value(nuevo_config, "QUANTUM_RR"); // leo tiempo maximo bloqueado
	    listaDispositivos = config_get_array_value(nuevo_config, "DISPOSITIVOS_IO");
		listaTiempos = config_get_array_value(nuevo_config, "TIEMPOS_IO");

	    t_configuracion_kernel *datos = malloc(sizeof(t_configuracion_kernel)); // creo estructura de datos de conexion
	    datos->ip_kernel = malloc(strlen(ip_kernel) + 1);
	   	strcpy(datos->ip_kernel, ip_kernel);
	    datos->ip_memoria = malloc(strlen(ip_memoria) + 1);
	    strcpy(datos->ip_memoria, ip_memoria);
	    datos->puerto_memoria = malloc(strlen(puerto_memoria) + 1);
	    strcpy(datos->puerto_memoria, puerto_memoria);
	    datos->ip_cpu = malloc(strlen(ip_cpu) + 1);
	    strcpy(datos->ip_cpu, ip_cpu);
	    datos->puerto_cpu_dispatch = malloc(strlen(puerto_cpu_dispatch) + 1);
	    strcpy(datos->puerto_cpu_dispatch, puerto_cpu_dispatch);
	    datos->puerto_cpu_interrupt = malloc(strlen(puerto_cpu_interrupt) + 1);
	    strcpy(datos->puerto_cpu_interrupt, puerto_cpu_interrupt);
	    datos->puerto_kernel = malloc(strlen(puerto_kernel) + 1);
	    strcpy(datos->puerto_kernel, puerto_kernel);
	    datos->puerto_escucha = malloc(strlen(puerto_escucha) + 1);
	    strcpy(datos->puerto_escucha, puerto_escucha);
	    datos->algoritmo_planificacion = malloc(strlen(algoritmo_planificacion) + 1);
	    strcpy(datos->algoritmo_planificacion, algoritmo_planificacion);
	    datos->grado_max_multiprogramacion = (intptr_t) malloc(sizeof(int));
	    datos->grado_max_multiprogramacion = grado_max_multiprogramacion;
	    datos->quantum_rr = (intptr_t) malloc(sizeof(int));
	    datos->quantum_rr = quantum_rr;
	    datos->dispositivos_io = listaDispositivos;
	    datos->tiempos_io = listaTiempos;

	    config_destroy(nuevo_config); // libero la memoria del config
	    return datos;
	}
