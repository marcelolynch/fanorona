#include "fanorona.h"
#include <stdio.h>

tFlag leerSN(void);
tFlag validarFmtNombre (char destino[], char origen[], int dimOrigen);
void imprimirError(tFlag error);
int getlinea(char str[], int dim);

tFlag pedirGuardar (char nombre[]) {
	char str[STR_DIM];
	int n;
	tFlag quiereGuardar, esValido = 1;

	printf("Desea guardar su juego antes de salir?\n");

	quiereGuardar = leerSN();

	if (!quiereGuardar)
		return 0;
	
	printf("Escriba el nombre con el que quiere guardar su juego. (Max %d caracteres)\n", MAX_NOM);

	do {
		imprimirError(esValido);
		n = getlinea(str, STR_DIM);
		esValido = validarFmtNombre (nombre, str, n);
	} while (esValido < 0);

	return esValido;
}
