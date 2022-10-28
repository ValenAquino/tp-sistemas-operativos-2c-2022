#include "../include/planificadorCortoPlazo.h"

extern t_list *procesosReady;
extern t_configuracion_kernel *config;

void planificar() {

	switch (config->algoritmo_planificacion) {
		case FIFO:
			break;
		case RR:
			break;
		case FEEDBACK:
			break;
	}

}


