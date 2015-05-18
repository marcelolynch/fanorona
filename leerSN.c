/* FRONT_END 
** Pide al usuario S o N. (si o no)
** Devuelve 1 o 0
*/

#include <stdio.h>
#include "fanorona.h"

#define SI 1
#define NO 0

int getlinea(char s[], int dim);

tFlag leerSN (void) {
	char c;
	char str[3];
	tFlag hayError = 0;

	do {
		printf("Ingrese S o N.\n");

		getlinea(str, 3); /* str de dimensión 3 así si se ingresa más de S o N str[1] será distinto de '\0' */
		c = toupper(str[0]);	
		hayError = 1;
	} while( (c!= 'S' && c!= 'N') || str[1] != '\0');
	
	return c == 'S' ? SI : NO;
}
