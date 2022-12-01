#include "../include/configuracion.h"

extern t_log* logger_debug;

t_configuracion_kernel* procesar_config(t_config *nuevo_config) {
	char **listaDispositivos;
	char **listaTiempos;

	char *ip_kernel = config_get_string_value(nuevo_config, "IP_KERNEL");                             // leo ip de kernel
	char *ip_memoria = config_get_string_value(nuevo_config, "IP_MEMORIA");                           // leo ip de memoria
	char *puerto_memoria = config_get_string_value(nuevo_config, "PUERTO_MEMORIA");                   // leo puerto
	char *ip_cpu = config_get_string_value(nuevo_config, "IP_CPU");                                   // leo ip de cpu
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
	
	datos->ip_kernel = malloc(strlen(ip_kernel) + size_caracter_fin_de_cadena);
	strcpy(datos->ip_kernel, ip_kernel);
	
	datos->ip_memoria = malloc(strlen(ip_memoria) + size_caracter_fin_de_cadena);
	strcpy(datos->ip_memoria, ip_memoria);
	
	datos->puerto_memoria = malloc(strlen(puerto_memoria) + size_caracter_fin_de_cadena);
	strcpy(datos->puerto_memoria, puerto_memoria);
	
	datos->ip_cpu = malloc(strlen(ip_cpu) + size_caracter_fin_de_cadena);
	strcpy(datos->ip_cpu, ip_cpu);
	
	datos->puerto_cpu_dispatch = malloc(strlen(puerto_cpu_dispatch) + size_caracter_fin_de_cadena);
	strcpy(datos->puerto_cpu_dispatch, puerto_cpu_dispatch);
	
	datos->puerto_cpu_interrupt = malloc(strlen(puerto_cpu_interrupt) + size_caracter_fin_de_cadena);
	strcpy(datos->puerto_cpu_interrupt, puerto_cpu_interrupt);
	
	datos->puerto_escucha = malloc(strlen(puerto_escucha) + size_caracter_fin_de_cadena);
	strcpy(datos->puerto_escucha, puerto_escucha);
	
	datos->algoritmo_planificacion = algoritmo_planificacion;
	datos->grado_max_multiprogramacion = grado_max_multiprogramacion;
	datos->quantum_rr = quantum_rr;
	
	datos->dispositivos = crear_lista_dispositivos(listaTiempos, listaDispositivos);

	config_destroy(nuevo_config); // libero la memoria del config
	return datos;
}

//dispositivos get_dispositivo(char* dispositivos) {
//	if(strcmp("DISCO", dispositivos) == 0)
//		return DISCO;
//	if(strcmp("IMPRESORA", dispositivos) == 0)
//		return IMPRESORA;
//
//	return 9999;
//}

t_list* crear_lista_dispositivos(char **tiempos, char **dispositivos) {
	t_list* lista = list_create();
	int indice_lista_disp = 2; // los dos primeros son los de consola

	void cargar_nombre(char* nombre) {
		ts_dispositivo *disp = malloc(sizeof(ts_dispositivo));

		disp->nombre = string_new();
		string_append(&(disp->nombre), nombre);

		list_add(lista, disp);
	}

	void cargar_tiempos(char *tiempo) {
		ts_dispositivo *disp = list_get(lista, indice_lista_disp);
		disp->tiempo_retardo = atoi(tiempo);
		indice_lista_disp++;
	}

	void inicializar_mutex_dispositivo(void *arg) {
		ts_dispositivo *disp = (ts_dispositivo *) arg;
		
		sem_init(&(disp->sem_dispositivo), 1, 1);
	}

	ts_dispositivo *teclado = malloc(sizeof(ts_dispositivo));
	teclado->nombre = "TECLADO";
	teclado->tiempo_retardo = 0;

	ts_dispositivo *pantalla = malloc(sizeof(ts_dispositivo));
	pantalla->nombre = "PANTALLA";
	pantalla->tiempo_retardo = 0;

	list_add(lista, teclado); // para que su indice sea 0
	list_add(lista, pantalla);// para que su indice sea 1

	string_iterate_lines(dispositivos, cargar_nombre);
	string_iterate_lines(tiempos, cargar_tiempos);
	list_iterate(lista, (void*) inicializar_mutex_dispositivo);

    return lista;
}

int obtener_tiempo_io(int indice_disp, t_list* lista_dispositivos) {
	ts_dispositivo *dispositivo = list_get(lista_dispositivos, indice_disp);

	return dispositivo->tiempo_retardo;
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
	
	log_debug(
		logger_debug,
		"\nLeyendo de config: \n"
		"IP_MEMORIA: %s \n"
		"PUERTO_ESCUCHA: %s \n"
		"PUERTO_MEMORIA: %s \n"
		"IP_CPU: %s \n"
		"PUERTO_CPU_DISPATCH: %s \n"
		"PUERTO_CPU_INTERRUPT: %s \n"
		"ALGORITMO_PLANIFICACION: %s \n"
		"GRADO_MAX_MULTIPROGRAMACION: %d \n"
		"QUANTUM_RR: %d \n",
		config->ip_memoria,
		config->puerto_escucha,
		config->puerto_memoria,
		config->ip_cpu,
		config->puerto_cpu_dispatch,
		config->puerto_cpu_interrupt,
		str_algoritmo(config->algoritmo_planificacion),
		config->grado_max_multiprogramacion,
		config->quantum_rr
	);

	void log_disp(void* arg) {
		ts_dispositivo *disp = (ts_dispositivo *) arg;
		log_debug(logger_debug, "<Disp: %s>|<T.Ret: %d>", disp->nombre, disp->tiempo_retardo);
	}

	list_iterate(config->dispositivos, (void*)log_disp);
}

void liberar_configuracion_kernel(t_configuracion_kernel* config_kernel) {
    free(config_kernel->ip_memoria);
    free(config_kernel->ip_kernel);
    free(config_kernel->puerto_memoria);
    free(config_kernel->ip_cpu);
    free(config_kernel->puerto_cpu_dispatch);
    free(config_kernel->puerto_cpu_interrupt);
    free(config_kernel->puerto_escucha);
    // ↓ hay que limpiar mas campos
    list_destroy_and_destroy_elements(config_kernel->dispositivos, free);
    free(config_kernel);
}
