/* FRONT-END
** pide una especificación de captura
** devuelve APPROACH o WITHDRAWAL. 
** mejor que reciba un puntero de tMovimiento y lo modifique ahi?
*/

#include <stdio.h>
#include "fanorona.h"

int getlinea(char str[], int dim);
enum tCaptura leerCaptura (const char str[]);

enum tCaptura pedirCaptura (void) {
	char str[5];
	int tipoCaptura;

	printf("Especifique algún tipo de captura, pues el movimiento que quiere realizar admite tanto captura por acercamiento como por alejamiento.\n");

	do {
		printf("Ingrese [w] o [a] > ");
		getlinea(str, 5);
		tipoCaptura = leerCaptura(str);
	} while (tipoCaptura == ERROR);
	
	return tipoCaptura;
}
/*
int main(void) {
	int algo = pedirCaptura();

	switch(algo) {
	case APPROACH: printf("buen approach\n"); break;
	case WITHDRAWAL: printf("withdrawal es de cagon\n"); break;
	default: printf("si se imprime hice algo mal\n"); break;
	}

	return 0;
}
*/
