#include "../include/configuracion.h"

extern t_log* logger;

t_configuracion_kernel* procesar_config(char *config_path) {
	char **listaDispositivos;
	char **listaTiempos;
	t_config* nuevo_config = config_create(config_path);

	if (nuevo_config== NULL) {
			log_error(logger, "No se pudo abrir el archivo de configuracion en ese path");
			exit(EXIT_FAILURE);
	}

	char *ip_kernel = config_get_string_value(nuevo_config, "IP_KERNEL");                             // leo ip de kernel
	char *ip_memoria = config_get_string_value(nuevo_config, "IP_MEMORIA");                           // leo ip de memoria
	char *puerto_memoria = config_get_string_value(nuevo_config, "PUERTO_MEMORIA");                   // leo puerto
	char *ip_cpu = config_get_string_value(nuevo_config, "IP_CPU");                                   // leo ip de cpu
	char *puerto_kernel = config_get_string_value(nuevo_config, "PUERTO_KERNEL");                     // leo ip de kernel
	char *puerto_cpu_dispatch = config_get_string_value(nuevo_config, "PUERTO_CPU_DISPATCH");         // leo puerto de cpu dispatch
	char *puerto_cpu_interrupt = config_get_string_value(nuevo_config, "PUERTO_CPU_INTERRUPT");       // leo puerto de cpu interrupt
	char *puerto_escucha = config_get_string_value(nuevo_config, "PUERTO_ESCUCHA");                   // leo puerto de escucha

	char *algoritmo_planificacion_str = config_get_string_value(nuevo_config, "ALGORITMO_PLANIFICACION"); // leo algoritmo de planificacion
	t_algoritmo_planificacion algoritmo_planificacion = procesar_algoritmo(algoritmo_planificacion_str);

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
	
	datos->algoritmo_planificacion = algoritmo_planificacion;
	datos->grado_max_multiprogramacion = grado_max_multiprogramacion;
	datos->quantum_rr = quantum_rr;
	
	// Lista de arrays [ dispositivo tiempo, dispositivo tiempo];
	datos->tiempos_io = procesar_tiempos_io(listaTiempos, listaDispositivos);

	config_destroy(nuevo_config); // libero la memoria del config
	return datos;
}

dispositivos get_dispositivo(char* dispositivos) {
	if(strcmp("DISCO", dispositivos) == 0) 
		return DISCO;
	if(strcmp("IMPRESORA", dispositivos) == 0) 
		return IMPRESORA;

	return 9999; 
}

t_list* procesar_tiempos_io(char **tiempos, char **dispositivos) {
	t_list* lista = list_create();

	for (int i = 0; i<2; i++) {
    	int *tiempos_io = malloc(sizeof(int) * 2);

        tiempos_io[0] = get_dispositivo(dispositivos[i]);
		tiempos_io[1] = atoi(tiempos[i]);
		list_add(lista, tiempos_io);
    }

    return lista;
}

t_algoritmo_planificacion procesar_algoritmo(char* algoritmo) {
    if(strcmp("FIFO", algoritmo) == 0)
        return FIFO;

    if(strcmp("RR", algoritmo) == 0)
        return RR;

    if(strcmp("FEEDBACK", algoritmo) == 0)
        return FEEDBACK;

    log_error(logger, "El algoritmo '%s' es incorrecta", algoritmo);
    exit(EXIT_FAILURE);
}

void test_read_config(t_configuracion_kernel* config) {
	int *dispo1 = malloc(sizeof(int) * 2);
	int *dispo2 = malloc(sizeof(int) * 2); 
	
	dispo1 = list_get(config->tiempos_io, 0);
	dispo2 = list_get(config->tiempos_io, 1);
	
	log_debug(
		logger,
		"Leyendo de config: \n"
		"IP_MEMORIA: %s \n"
		"PUERTO_ESCUCHA: %s \n"
		"PUERTO_MEMORIA: %s \n"
		"IP_CPU: %s \n"
		"PUERTO_KERNEL: %s \n"
		"PUERTO_CPU_DISPATCH: %s \n"
		"PUERTO_CPU_INTERRUPT: %s \n"
		"ALGORITMO_PLANIFICACION: %s \n"
		"GRADO_MAX_MULTIPROGRAMACION: %d \n"
		"QUANTUM_RR: %d \n"
		"TIEMPO 1: %d, %dms \n"
		"TIEMPO 2: %d, %dms \n",
		config->ip_memoria,
		config->puerto_escucha,
		config->puerto_memoria,
		config->ip_cpu,
		config->puerto_kernel,
		config->puerto_cpu_dispatch,
		config->puerto_cpu_interrupt,
		str_algoritmo(config->algoritmo_planificacion),
		config->grado_max_multiprogramacion,
		config->quantum_rr,
		dispo1[0], dispo1[1],
		dispo2[0], dispo2[1]
	);
}

int obtener_tiempo_io(dispositivos disp_que_busco, t_list* lista_dispositivos) {

	// nested function
 	bool get_time(void *element) {
		int *array_dispositivo_tiempo = element;
		return array_dispositivo_tiempo[0] == disp_que_busco;
	}

	int *array_dispositivo_tiempo = list_find(lista_dispositivos, get_time);

	return array_dispositivo_tiempo[1];
}