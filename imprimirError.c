/* FRONT-END */

#include <stdio.h>
#include "fanorona.h"

void imprimirError(tFlag error) {
	if (error >= 0) /* no hay error */
		return;

	switch (error) {
	case ERR_FMT: 
		printf("Error: no se introdujo un tipo de jugada válido.\n"); 
		break;
	case ERR_FMT_SAVE: 
		printf("Error: no se introdujo un nombre válido; debe contener al menos un carácter distinto de espacio.\n"); 
		break;
	case ERR_FMT_MOV1: 
		printf("Error: no se introdujo un movimiento con el formato adecuado o los números ingresados son mayores de lo necesario.\n");
		printf("El formato debe ser: M [Fo,Co][Fd,Cd][a/w] siendo la especificación de captura opcional en casos donde ésta no sea ambigua.\n");
		break;
	case ERR_FMT_MOV2: 
		printf("Error: no se especificó la captura correctamente.\n");
		printf("La misma debe añadirse como [a] para approach o [w] para withdrawal inmediatamente seguido de la coordenada destino.\n");
		break;
	case ERR_MOV_ORIG:
		printf("Error: la casilla elegida no contiene una ficha de su color.\n");
		break;
	case ERR_MOV_DEST:
		printf("Error: la casilla a la que quiere moverse no se encuentra vacía.\n");
		break;
	case ERR_MOV_TOC:
		printf("Error: no puede moverse a una casilla ya visitada en un mismo turno.\n");
		break;
	case ERR_MOV_DIR:
		printf("Error: no puede moverse en la misma dirección del movimiento anterior.\n");
		break;
	case ERR_MOV_PAIKA:
		printf("Error: no puede mover una ficha que no puede realizar una captura cuando existen otras que sí pueden.\n");
		break;	
	default:
		printf("Error desconocido\n");
		break;
	}
}
