#include "fanorona.h"
#include <stdio.h>

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
tFlag pedirGuardar(char nombre[]);
void actualizarTablero(tTablero * tablero, enum tDireccion direccion, tMovimiento mov);
int validarMovimiento(char jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia);
void imprimirError(tFlag error);
enum tCaptura pedirCaptura (void);
void pedirCadena(tMovimiento *mov);
int jugadaObligada(tTablero * tablero, int jugador, tCoordenada origen);
int paika(char jugador, tTablero * tablero);

void limpiarTocadas(tTablero * tablero){
	int i,j;
	for(i=0; i<tablero->filas ; i++)
		for(j=0; j<tablero->cols ; j++){
			if(tablero->matriz[i][j].estado == TOCADA)
				tablero->matriz[i][j].estado = VACIO;
		}
}


int main(){
	char nombre[MAX_NOM];
	tMovimiento mov;
        tTablero tablero;
	tFlag jugada, quiereGuardar=0, obligado=0;
	int jugador=0;
	int movimiento;
	enum tDireccion dir=NULA;
	int a,b;
	PedirDimensiones(&tablero);
	tablero=GenerarTablero(tablero.filas,tablero.cols);
	ImprimirTablero(&tablero);

	do{	
		movimiento=-1;
		do{
			
			if (obligado)
				pedirCadena(&mov);
			else
				jugada=pedirJugada(&mov, nombre);
				
			if(jugada==MOV){	
				movimiento=validarMovimiento(jugador, &tablero, mov, &dir);
				if (movimiento == AMBOS) {
					mov.tipoMov = pedirCaptura();
				}
				else if(movimiento==APPROACH || movimiento==WITHDRAWAL){
					mov.tipoMov=movimiento;
					}
				else if (movimiento != NINGUNO)
					imprimirError(movimiento);
					
				printf("\n MOVIMIENTO: %d\n OBLIGADO:%d\n", movimiento, obligado);
			}	
		}while(jugada==MOV && movimiento<0); /*ERROR*/
	
	
		if(jugada==MOV){
			printf("TipoMov: %d\n", mov.tipoMov);
			actualizarTablero(&tablero, dir, mov);	
			ImprimirTablero(&tablero);
			obligado = 0;			

			if (movimiento != NINGUNO) /* no puede encadenar luego de paika */
				obligado = jugadaObligada(&tablero, jugador, mov.coordDest);

			if (obligado) {
				mov.coordOrig.fil = mov.coordDest.fil; /*al tener que concatenar una jugada, el origen es el destino de antes*/ 
				mov.coordOrig.col = mov.coordDest.col;
			}
			else { /*Cambio de turno*/
				printf("Cambio\n");
				jugador=!jugador; /*Cambia*/
				printf("\nLe toca al jugador %s\n", jugador?"negro":"blanco");
				dir = NULA; /*Ninguna*/
				limpiarTocadas(&tablero);
			}
			
			for(a=0; a<tablero.filas ; a++)
			for(b=0; b<tablero.cols ; b++){
				if(tablero.matriz[a][b].estado==TOCADA)
					printf("Tocada: %d, %d\n",a+1,b+1);
				}

		}

		if (jugada == QUIT)
			quiereGuardar = pedirGuardar(nombre);
		if (jugada == SAVE || quiereGuardar) {
			/* poner lo del SAVE aca */
			printf("Se ha guardado su juego con el nombre '%s'\n", nombre);
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
	else if (mov.tipoMov != NINGUNO)
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
		default:
		break;
        }
}
