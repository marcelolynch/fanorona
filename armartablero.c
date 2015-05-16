#include <stdio.h>
#include "getnum.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct	{	char ocupante; /* 'B' blanco o 'N' negro o 'o' */
					char tipo;  /* 'D' debil o 'F' fuerte */
					
				}tCasilla;
				
typedef tCasilla ** tablero;


void PedirDimensiones(int *filas, int *columnas)
{	int cantfils, cantcols, impar;
	char c;
	do	{	impar=1;
			printf("Por favor ingrese dimensiones validas para tablero con el que desea Jugar\n");
			cantfils= getint("Cantidad de Filas (impar entre 3 y 19) : ");
			cantcols= getint("Cantidad de Columnas (impar entre 3 y 19):  ");
			putchar('\n');
			if( cantfils %2 == 1 && cantcols % 2==1 && cantfils<=cantcols)
				{	if (cantfils >=3 && cantfils <=19 && cantcols >0 && cantcols <=19 )
					{	printf("Las dimensiones del tablero seran : %d x %d\n\n", cantfils,cantcols);
						do{	printf("¿Desea cambiarlas? \nIngrese S si lo desea o N para jugar con esas dimensiones\n");
						c=toupper(getchar());
				
						} while( c!= 'S' && c!= 'N');
					}
				}
			else impar=0;
		}
		while (cantfils <3 || cantfils >19 || cantcols <3 || cantcols >19 || impar== 0 || c== 'S' || cantfils>cantcols );
	
			
	
	*filas=cantfils;
	*columnas=cantcols;
	return;
}

tCasilla** GenerarTablero( int fils, int cols)
{	int i, j, hubocentro=0, ultima=1;
	tCasilla **tablero;

	tablero=malloc( fils *sizeof(tCasilla));
	
	if (tablero== NULL)		
			return NULL;		/*FALTA LA FUNCION FREE */
		
	
	
	for( i=0; i< fils; i++)
	{	tablero[i]= malloc(cols*sizeof(tCasilla));
		
		if( tablero[i] ==NULL)	
			return NULL;									/*FALTA LA FUNCION FREE */
		
		
		for( j=0; j<cols; j++)
			{	
				if( i== 0 || i== fils-1 || j==0 || j==cols-1)
					tablero[i][j].tipo= 'D';
				else if ( (i%2== 0 && j%2==0) || (i%2==1 && j%2==1))
					tablero[i][j].tipo= 'F';
				else tablero[i][j].tipo= 'D';
			
				
				if ( i< fils/2)
					tablero[i][j].ocupante = 'N';
				else if ( i > (fils)/2)
					tablero[i][j]. ocupante= 'B';
				else if ( i == (fils)/2)
					{	if (j==0)
							tablero[i][j]. ocupante= 'N';
						else if ( j== cols/2)
							{	tablero[i][j].ocupante= '0';
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
