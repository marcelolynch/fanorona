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

void PedirDimensiones(int *filas, int *columnas){
	/*FUNCION DEL FRONT END*/
	tFlag hayError;
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

/* LAS FUNCIONES QUE SIGUEN SON DEL BACK END */

void rellenarTablero(tTablero tablero, int fils, int cols){
	int i,j;
	int postCentral=0;
	for(i=0; i<fils ; i++){
	for(j=0; j<cols; j++)
		{	
		if (i%2 == j%2)
			tablero[i][j].tipo= FUERTE;
		else
			 tablero[i][j].tipo= DEBIL;

		
		if (i < fils/2)
			tablero[i][j].ocupante = NEGRO;
		else if (i > fils/2)
			tablero[i][j].ocupante= BLANCO;
	
		else if (i == fils/2 && j != cols/2) /*Fila central (menos casilla central) */
		{	
			if (j%2==0)
				tablero[i][j].ocupante = postCentral ? BLANCO:NEGRO; /*No hay simetria con respecto a la central; se invierte la paridad luego de la misma*/
			else
				tablero[i][j].ocupante = postCentral ? NEGRO:BLANCO;
			
		}
		else{	/*Casilla central*/
			tablero[i][j].ocupante= VACIO;
			postCentral=1; /*Relevante para las otras piezas*/
		}	
	}	
	}
	
	return;
}

void liberarTablero(tTablero tablero, int n){
	
	int i;
	for(i=0; i<n ; i++)
		free(tablero[i]);
	
	free(tablero);
			  
}

tTablero GenerarTablero( int fils, int cols)
{	int i;
	
	tCasilla **tablero;

	tablero=malloc(fils *sizeof(*tablero));

	if (tablero == NULL)
		return NULL;
		
	for(i=0; i< fils; i++)
	{	
		tablero[i]= malloc(cols*sizeof(tCasilla));
		
		if(tablero[i] ==NULL){
			liberarTablero(tablero, i);
			return NULL;
		}
	}

	rellenarTablero(tablero, fils, cols);
	
	return tablero;

}

/* FIN BACK END*/

/*LAS FUNCIONES QUE SIGUEN SON DE FRONT END*/
void ImprimirTablero ( tTablero tablero, int fils, int cols)
{	int i, j;
	static char idColor[]={'B', 'N', 'O'} /*BLANCO, NEGRO, VACIO*/

	for(i=0; i<fils; i++)
		{	putchar('\n');
			for(j=0; j<cols; j++)
				{	if (tablero[i][j].tipo==DEBIL)
						printf("%c   ", tolower( idColor[ tablero[i][j].ocupante ] ); /*BLANCO=0, NEGRO=1, VACIO=2*/
					else 
						printf("%c   ", idColor[ tablero[i][j].ocupante ]);
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
