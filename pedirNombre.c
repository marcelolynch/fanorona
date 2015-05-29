#include "fanorona.h"
#include <stdio.h>

tFlag leerSN(void);
tFlag validarFmtNombre (char destino[], char origen[], int dimOrigen);
void imprimirError(tFlag error);
int getlinea(char str[], int dim);
void imprimirError(tFlag error);

void pedirNombre (char nombre[]) {
	char str[STR_DIM];
	int n;
	tFlag esValido = 1;

	printf("Escriba el nombre con el que quiere guardar su juego. (Max %d caracteres)\n", MAX_NOM);

	do {
		imprimirError(esValido); /* en una primera instancia no imprime nada, pues esValido es mayor o igual a 0*/
		n = getlinea(str, STR_DIM);
		esValido = validarFmtNombre (nombre, str, n);
	} while (esValido < 0);

	return;
}
