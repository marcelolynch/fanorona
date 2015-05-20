#include "fanorona.h"
#include <stdio.h>
#define MAX_NOMBRE 25
void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion);
void PedirDimensiones(tTablero * tablero);
tTablero GenerarTablero(int fils, int cols);
void ImprimirTablero ( tTablero * tablero );
tFlag pedirJugada(tMovimiento *mov, char *nombre);
void liberarTablero(tTablero * tablero, int n);
enum tCaptura leerCaptura (const char str[]);
void PedirDimensiones(tTablero * tablero);
tTablero GenerarTablero(int fils, int cols);
tFlag pedirJugada(tMovimiento *mov, char *nombre);
void actualizarTablero(tTablero * tablero, enum tDireccion direccion, tMovimiento mov);
int validarMovimiento(char jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia, tFlag limpiar, tFlag * proxObligado);
void imprimirError(tFlag error);

enum tCaptura leerCaptura (const char str[]);

int main(){
	char str[MAX_NOMBRE];
	tMovimiento mov;
        tTablero tablero;
	tFlag jugada,limpiar=0,obligado=1;
	int jugador=0;
	int movimiento;
	enum tDireccion dir=-1;

	PedirDimensiones(&tablero);
	tablero=GenerarTablero(tablero.filas,tablero.cols);
	ImprimirTablero(&tablero);

	do{	
		movimiento=-1;	
		do{
			
			if(!obligado){
				printf("Cambio\n");
				jugador=!jugador; /*Cambia*/
				printf("\nLe toca al jugador %s\n", jugador?"negro":"blanco");
				limpiar=1;
				}
			if(movimiento<0)
				jugada=pedirJugada(&mov, str);
				
			if(jugada==MOV){	
				movimiento=validarMovimiento(jugador, &tablero, mov,  &dir, limpiar, &obligado);
				imprimirError(movimiento);
				printf("\n MOVIMIENTO: %d\n OBLIGADO:%d\n", movimiento, obligado);
				}	
				else
					limpiar=0;
		}while(jugada==MOV && movimiento<0); /*ERROR*/
	
	
		if(jugada==MOV){
			printf("TipoMov: %d\n", mov.tipoMov);
			actualizarTablero(&tablero, dir, mov);	
			ImprimirTablero(&tablero);
		}
	
	}while(jugada!=QUIT);
	
	return 0;

}

void actualizarTablero(tTablero * tablero, enum tDireccion direccion, tMovimiento mov){
	int i, j;
	int dirFil, dirCol;
	int fini, cini;
	int jugador = tablero->matriz[mov.coordOrig.fil][mov.coordOrig.col].ocupante;
	int enemigo = !jugador;
	incrementoSegunDir(&dirFil, &dirCol, direccion);
	
	if(mov.tipoMov==WITHDRAWAL){
		fini=mov.coordOrig.fil - dirFil; /*De donde empieza a comer*/
		cini=mov.coordOrig.col - dirCol;
		dirFil = -dirFil;	/*Se invierte la direccion a recorrer*/
		dirCol = -dirCol;
	}

	else if(mov.tipoMov==APPROACH){
		fini=mov.coordDest.fil + dirFil; /*De donde empieza a comer*/
		cini=mov.coordDest.col + dirCol;
		}
	else
		printf("ERROOOOOOOOOR\n\a\a\a\a");
	printf("%d,%d -> %d,%d\n", mov.coordOrig.fil, mov.coordOrig.col, mov.coordDest.fil, mov.coordDest.col);
	tablero->matriz[mov.coordDest.fil][mov.coordDest.col].ocupante = jugador;
	tablero->matriz[mov.coordOrig.fil][mov.coordOrig.col].ocupante = VACIO;	/*Movi la ficha*/

	i=fini;
	j=cini;
	while((i<(tablero->filas) && i>=0 && j<(tablero->cols) && j>=0) && tablero->matriz[i][j].ocupante==enemigo){
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
                         *dirCol=-1;
                break;
                case SE: *dirFil=1;
                         *dirCol=1;
                break;
                case SO: *dirFil=1;
                         *dirCol=-1;
                break;
        }
}
