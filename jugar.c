#include "fanorona.h"
#include <stdio.h>

void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion);

int jugar(){


	return 0;

}

void actualizarTablero(tTablero * tablero, int jugador, enum tDireccion direccion, tMovimiento mov){
	int i, j;
	int dirFil, dirCol;
	int fini, cini;
	int enemigo = !jugador;
	incrementoSegunDir(&dirFil, &dirCol, direccion);

	if(mov.tipoMov==WITHDRAWAL){
		fini=mov.coordOrig.fil - dirCol; /*De donde empieza a comer*/
		cini=mov.coordOrig.col - dirCol;
		dirFil = -dirFil;	/*Se invierte la direccion a recorrer*/
		dirCol = -dirCol;
	}

	else if(mov.tipoMov==APPROACH){
		fini=mov.coordDest.fil + dirFil; /*De donde empieza a comer*/
		cini=mov.coordDest.col + dirCol;
		}

	tablero->matriz[mov.coordDest.fil][mov.coordDest.col].ocupante = jugador;
	tablero->matriz[mov.coordOrig.fil][mov.coordOrig.col].ocupante = VACIO;	/*Movi la ficha*/
	
	i=fini;
	j=cini;
	while((i<tablero->filas && i>=0 && j<tablero->cols && j>=0) && tablero->matriz[i][j].ocupante==enemigo){
		/*Mientras no me vaya del tablero y siga habiendo enemigos en la linea de captura*/
	
		tablero->matriz[i][j].ocupante = VACIO;
		
		i+=dirFil;
		j+=dirCol;
	}
	
}

void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion){

 switch(direccion){
                case N: *dirFil=-1;
                        *dirCol=0;
                break;
                case S: *dirFil=1;
                        *dirCol=0;
                break;
                case E: *dirFil=0;
                        *dirCol=1;
                break;
                case O: *dirFil=0;
                        *dirCol=-1;
                break;
                case NE: *dirFil=-1;
                         *dirCol=1;
                break;
                case NO: *dirFil=-1;
                         *dirCol=0;
                break;
                case SE: *dirFil=1;
                         *dirCol=1;
                break;
                case SO: *dirFil=1;
                         *dirCol=-1;
                break;
        }
}
