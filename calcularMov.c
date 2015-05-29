#include "fanorona.h"
#include <stdlib.h>
#define BLOQUE 20

typedef struct {
	tMovimiento * elems;
	size_t dim;
 } tVecMovs;	

void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion);
enum tCaptura hayComida (char jugador, tTablero *tablero, tCoordenada origen, enum tDireccion direccion);
int aumentarPosibles(tTablero * tablero, tVecMovs * movsPosibles, tCoordenada casillaOrig, tFlag hayPaika);


int calcularMovCompu(tMovimiento * mov, tTablero * tablero, tFlag hayPaika, tFlag hayCadena){

	tVecMovs movsPosibles;
	int i,j;
	int eleccion;
	tCoordenada casillaOrig;
        
	movsPosibles.dim = 0;
	movsPosibles.elems = NULL;

	if(!hayCadena){

		for(i=0 ; i<tablero->filas ; i++)
			for(j=0 ; j<tablero->cols ; j++)
				if(tablero->matriz[i][j].ocupante == NEGRO){
					casillaOrig.fil=i;
					casillaOrig.col=j;
					if( aumentarPosibles(tablero, &movsPosibles, casillaOrig, hayPaika) != 0)
						return 1;		
					}
	}
	else{
		/*Si estoy encadenando, la coordenada de origen esta ya determinada en mov que me pasan como parametro*/
		aumentarPosibles(tablero, &movsPosibles, mov->coordOrig, hayPaika);
	}

	eleccion = rand()%movsPosibles.dim;
	*mov = movsPosibles.elems[eleccion];
	return 0;
}

int aumentarPosibles(tTablero * tablero, tVecMovs * movsPosibles, tCoordenada casillaOrig, tFlag hayPaika){
        
	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	int dirsPosibles, dirFil, dirCol;	
	int n;
	tMovimiento * aux;
	enum tCaptura tipoComida;
	enum tDireccion dir;
	
	dirsPosibles = tablero->matriz[casillaOrig.fil][casillaOrig.col].tipo==FUERTE ? 8 : 4;
	
	for(n=0; n<dirsPosibles ; n++){
		dir = direcciones[n];
		incrementoSegunDir(&dirFil, &dirCol, dir);
                #define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols)             
						
		if(!FDERANGO(casillaOrig.fil + dirFil, casillaOrig.col + dirCol) && tablero->matriz[casillaOrig.fil + dirFil][casillaOrig.col + dirCol].estado != TOCADA ){
			if( (tipoComida = hayComida(NEGRO, tablero, casillaOrig, dir)) != NINGUNO || hayPaika ){

				if(movsPosibles->dim % BLOQUE == 0){
					aux=realloc(movsPosibles->elems, (movsPosibles->dim + BLOQUE)*sizeof(*aux));
			
					if(aux==NULL)
						return 1;
				
					movsPosibles->elems = aux;
					}
	
				movsPosibles->elems[movsPosibles->dim].coordOrig.fil = casillaOrig.fil;
				movsPosibles->elems[movsPosibles->dim].coordOrig.col = casillaOrig.col;
				movsPosibles->elems[movsPosibles->dim].coordDest.fil = casillaOrig.fil+dirFil;
				movsPosibles->elems[movsPosibles->dim].coordDest.col = casillaOrig.col+dirCol;

				if(tipoComida == AMBOS) /*Elijo una al azar*/
					tipoComida = rand()%2?APPROACH:WITHDRAWAL;

				movsPosibles->elems[movsPosibles->dim].tipoMov = tipoComida;
	
				movsPosibles->dim++;
			}
		}
	}

	return 0;
}
