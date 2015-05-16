#include <stdio.h>
#include "fanorona.h"
#include "getnum.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MIN_DIM 3
#define MAX_DIM 19

#define ES_IMPAR(a) ((a) % 2 == 1)
#define ES_DIM_VALIDA(a, b) ( (a) >= MIN_DIM && (a) <= MAX_DIM && (b) >= MIN_DIM && (b) <= MAX_DIM )

int getlinea(char str[], int dim);

void PedirDimensiones(int *filas, int *columnas)
{	tFlag hayError;
	int cantfils, cantcols, impar;
	char str[2];
	char c;

	do {
		hayError = 0;
		printf("Por favor, ingrese las dimensiones del tablero con el que desea jugar:\n");

		do {
			if (hayError)
				printf("No ingresó dimensiones correctas. Ingrese nuevamente:\n");
			cantfils= getint("Cantidad de Filas (impar entre %d y %d): ", MIN_DIM, MAX_DIM); 
			cantcols= getint("Cantidad de Columnas (impar entre %d y %d): ", MIN_DIM, MAX_DIM);
			putchar('\n');
			hayError = 1;
		} while ( !ES_IMPAR(cantfils) || !ES_IMPAR(cantcols) || !ES_DIM_VALIDA(cantfils, cantcols) || cantfils > cantcols);
		/* OJO si se ingresan mal las filas pide columnas igual. */

		hayError = 0;

		printf("Las dimensiones del tablero serán: %d x %d\n\n", cantfils,cantcols);
		printf("¿Desea continuar?\nIngrese S si es así o N para ingresar nuevas dimensiones.\n");

		do {
			if (hayError)
				printf("Ingrese unicamente S o N seguido de un ENTER\n");
			getlinea(str, 2);
			c = toupper(str[0]);	
		} while( c!= 'S' && c!= 'N');	

	} while (c == 'N'); /* se desean ingresar nuevas dimensiones */

	*filas=cantfils;
	*columnas=cantcols;
	return;
}



void rellenarTablero(tCasilla ** tablero, int fils, int cols){
	int i,j;
	int ultima=1;
	for(i=0; i<fils ; i++){
	for(j=0; j<cols; j++)
		{	
		if( i== 0 || i== fils-1 || j==0 || j==cols-1)
			tablero[i][j].tipo= 'D';
		else if ( (i%2== 0 && j%2==0) || (i%2==1 && j%2==1))
			tablero[i][j].tipo= 'F';
		else tablero[i][j].tipo= 'D';
	
		
		if ( i < fils/2)
			tablero[i][j].ocupante = 'N';
		else if ( i > fils/2)
			tablero[i][j].ocupante= 'B';
		else if ( i == fils/2)
		{
			if (j==0)
				tablero[i][j].ocupante= 'N';
			else if ( j== cols/2)
			{	
				tablero[i][j].ocupante= 'o';
				ultima= !(ultima);
			}
			else if( ultima==0)
				tablero[i][j].ocupante= 'B';
			else if (ultima==1)
				tablero[i][j].ocupante= 'N';
			ultima=!(ultima);
		}
		
	}		
	}
	
	return;
}

tCasilla** GenerarTablero( int fils, int cols)
{	int i;
	tCasilla **tablero;

	tablero=malloc( fils *sizeof(int));

	if (tablero== NULL)
		return NULL;		/*FALTA LA FUNCION FREE */
		
	for(i=0; i< fils; i++)
	{	tablero[i]= malloc(cols*sizeof(tCasilla));
		
		if( tablero[i] ==NULL)	
			return NULL;				/*FALTA LA FUNCION FREE */
		
	}

	rellenarTablero(tablero, fils, cols);
	
	return tablero;

}




void ImprimirTablero ( tCasilla ** tablero, int fils, int cols)
{	int i, j;

		for(i=0; i<fils; i++)
		{	putchar('\n');
			for(j=0; j<cols; j++)
				{	if (tablero[i][j].tipo== 'D')
						printf("%c   ", tolower(tablero[i][j].ocupante));
					else 
						printf("%c   ", tablero[i][j].ocupante);
				}
		}
		putchar('\n');
	return;		
}	


/*main para probarlas */
int main (void)
{	int fils, cols,c;
	tCasilla ** tablero;
	int i,j;
	
	PedirDimensiones(&fils,&cols);
	
	printf("Las dimensiones son : %d x %d \n", fils,cols);
	
	tablero= GenerarTablero (fils,cols);
	if (tablero != NULL)
		ImprimirTablero(tablero,fils,cols);
	
	
	return 0;
}
