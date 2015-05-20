#include "fanorona.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

void liberarTablero(tTablero * tablero, int n);

int guardarPartida(tTablero * tablero, int modo, int jugador, const char * nombre){
	FILE *f;
	int nfilas=tablero->filas;
	int ncols=tablero->cols;
	int i,j;

	if((f=fopen(nombre, "wb")) == NULL)
		return ERROR;
	fwrite(&modo, sizeof(int),1,f);
	fwrite(&jugador,sizeof(int),1,f);
	fwrite(&nfilas, sizeof(int),1,f);
	fwrite(&ncols, sizeof(int),1,f);
	for(i=0; i<nfilas ; i++)
		for(j=0; j<ncols ; j++){
			switch(tablero->matriz[i][j].ocupante){
				case BLANCO:
				fputc('&', f); break;
				case NEGRO:
				fputc('#', f); break;
				case VACIO:
				fputc('0', f); break;
			}
		}	

	return 0;
}

tTablero cargarPartida(int * modo, int * jugador, const char * nombre){
	/*asumo que los datos estan validados, el archivo no esta corrputo*/
	int fils,cols;
	int i,j;
	FILE * f;
	tTablero tablero;

	if((f=fopen(nombre,"rb"))==NULL)
		return tablero;

	fread(modo, sizeof(int),1, f);
	fread(jugador, sizeof(int),1,f);
	fread(&fils, sizeof(int), 1, f);
	fread(&cols, sizeof(int), 1, f);

	printf("\nMODO DE JUEGO:%d\nTURNO DEL JUGARDOR Nº%d\nDIMS:%dx%d\n\n",*modo,*jugador,fils,cols);

        tablero.matriz=malloc(fils*sizeof(*tablero.matriz));

        if (tablero.matriz == NULL)
                return tablero;

        for(i=0; i<fils; i++)
        {
                tablero.matriz[i]=malloc(cols*sizeof(tCasilla));

                if(tablero.matriz[i] ==NULL){
                        liberarTablero(&tablero, i);
                        return tablero;
                }
        }


	tablero.filas=fils;
	tablero.cols=cols;

	for(i=0; i<tablero.filas ; i++)
		for(j=0 ; j<tablero.cols ; j++){
			if (i%2 == j%2)
                        	tablero.matriz[i][j].tipo= FUERTE;
                	else
                        	 tablero.matriz[i][j].tipo= DEBIL;

			switch(fgetc(f)){
				case '0':
				tablero.matriz[i][j].ocupante=VACIO;
				break;	
				case '&':
				tablero.matriz[i][j].ocupante=BLANCO;
				break;	
				case '#':
				tablero.matriz[i][j].ocupante=NEGRO;
				break;	
				default:
				liberarTablero(&tablero, tablero.filas);
				return tablero;
				break;
 			}
		}

	return tablero;
}
