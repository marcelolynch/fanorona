/* FRON-END 
** entrada: puntero del tMovimiento movimiento anterior.
** salida: booleano. 0 si no se desea encadenar o 1 en caso contrario.
**	   Pide al usuario nueva dirección de destino y actualiza al tMovimiento de forma acorde.
*/

#include "fanorona.h"
#include <stdio.h>
#include <string.h>

#define STR_DIM 30
#define MIN_STR 5 /* mínima longitud de string de coordenada válida que puede escribir el usuario */
#define MAX_STR 10  /* máxima longitud de string de coordenada válida que puede escribir el usuario */

int getlinea(char str[], int dim);
tFlag leerSN(void);
tFlag validarMovFormato (const char str[], tMovimiento *mov);

tFlag quiereEncadenar (tMovimiento *mov) {
	tFlag deseaCadena, esValido = 1;
	char str[STR_DIM]; 
	char nuevoStr[15]; /* tamaño suficiente para evaluar si el usuario introdujo de más */
	int fo, co;
	int n;

	printf ("Desea encadenar un movimiento?\n");

	deseaCadena = leerSN();
	
	if (!deseaCadena)
		return 0;
	
	/* las coordenadas de origen nuevas son las del destino anterior */
	fo = mov->coordOrig.fil = mov->coordDest.fil; 
	co = mov->coordOrig.col = mov->coordDest.col;

	printf("Ingrese solo la coordenada de la casilla a la que desea moverse y el tipo de captura si es necesario\n");
	printf("Se imprimira su nueva casilla de origen.\n");

	do {
		imprimirError(esValido); /* en una primer instancia no imprimirá nada, pues esValido es mayor a 0 */

		printf("M [%d,%d]", fo, co);

		n = getlinea(nuevoStr, 15);

		if (n >= MIN_STR && n <= MAX_STR) {
			sprintf(str, "M [%d,%d]%s", fo, co, nuevoStr);
			esValido = validarMovFormato (str, mov); /* devuelve menor a 0 en caso de error */
		}
		else
			esValido = ERR_FMT_MOV1;

	} while (esValido < 0); /* si es menor a cero es error */

	return deseaCadena;
}

/* TESTEO */
int main(void) {
	tMovimiento mov;
	mov.coordOrig.fil = 4;
	mov.coordOrig.col = 7;
	mov.coordDest.fil = 9;
	mov.coordDest.col = 10;

	printf("Coordenada anterior:\n");
	printf("[%d,%d] [%d,%d]\n", mov.coordOrig.fil, mov.coordOrig.col, mov.coordDest.fil, mov.coordDest.col);

	if (quiereEncadenar(&mov)) {
		printf("Nueva coordenada:\n");
		printf("[%d,%d] [%d,%d]\n", mov.coordOrig.fil, mov.coordOrig.col, mov.coordDest.fil, mov.coordDest.col);
	}
	else
		printf("No quiere encadenar\n");

	return 0;
}
