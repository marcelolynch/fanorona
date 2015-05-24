#include <stdio.h>
#include "getnum.h"
#include "fanorona.h"

void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion);
void pedirDimensiones(tTablero * tablero);
tTablero generarTablero(int fils, int cols);
void imprimirTablero ( tTablero * tablero );
tFlag pedirJugada(tMovimiento *mov, char *nombre);
void liberarTodo(tCasilla ** tablero, int n);
enum tCaptura leerCaptura (const char str[]);
void pedirDimensiones(tTablero * tablero);
tFlag pedirJugada(tMovimiento *mov, char *nombre);
tFlag pedirGuardar(char nombre[]);
void actualizarTablero(tTablero * tablero, enum tDireccion direccion, tMovimiento mov);
int validarMovimiento(char jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia);
void imprimirError(tFlag error);
enum tCaptura pedirCaptura (void);
void pedirCadena(tMovimiento *mov);
int jugadaObligada(tTablero * tablero, int jugador, tCoordenada origen);
int paika(char jugador, tTablero * tablero);
int guardarPartida(tTablero * tablero, int modo, int jugador, const char * nombre);
tTablero cargarPartida(int * modo, int * jugador, const char * nombre);
int jugar(tTablero tablero, int modo, int jugador);
int meCapturan(tTablero *tablero, tCoordenada posicion, char jugador);

void limpiarTocadas(tTablero * tablero){
	int i,j;
	for(i=0; i<tablero->filas ; i++)
		for(j=0; j<tablero->cols ; j++){
			if(tablero->matriz[i][j].estado == TOCADA)
				tablero->matriz[i][j].estado = VACIO;
		}
}

#define SEGUIR 0
#define GANADOR_BLANCO 1
#define GANADOR_NEGRO 2
#define EMPATE 3
int estadoPostJugada(tTablero * tablero){
	int hayBlancos=0, hayNegros=0;
	int ocupante;
	int i,j,n;
	int hayMovimientos=0;
	tCoordenada origen;
	tCoordenada ady;
	enum tDireccion direcciones[]={N, S, E, O, NE, NO, SE, SO};
	int dir, dirsPosibles, dirFil, dirCol;
	int estado;

	for(i=0; i<tablero->filas ; i++)
	for(j=0; j<tablero->cols ; j++){

		if((ocupante = tablero->matriz[i][j].ocupante) == VACIO){
			/* Limpieza de las casillas tocadas */
			if(tablero->matriz[i][j].estado == TOCADA) 
                	      	tablero->matriz[i][j].estado = LIBRE;
		}
		else{ 
			if(ocupante==BLANCO){
				/*printf("Hay un blanco en %d, %d\n", i,j);*/
				hayBlancos=1;
			}
			else{
				hayNegros=1;
				/*printf("Hay un negro en %d, %d\n", i,j);*/
			}


			/*Voy a analiar si esta casilla esta en una posicion desfavorable (bloqueada) en la proxima movida
			** es decir, si no puede capturar nada en la siguiente movida que haga, y que no importa donde se mueva
			** alguien lo captura. Si todas las piezas (de ambos jugadores) cumplen con esto, el juego termina en empate. */
			origen.fil = i;
			origen.col = j;
			if(jugadaObligada(tablero, ocupante, origen)){
				hayMovimientos=1;
			}
			/*Si puede comer en la jugada posterior, la casilla no esta bloqueada
			** (esta obligada a comer aunque sea mala estrategia)
			** si la jugada no es obligada, analizo para cada direccion si la captura es inminente en caso de dirigirse a ella*/
			else{
        	        	dirsPosibles = tablero->matriz[i][j].tipo == FUERTE ? 8:4;
                		for(n=0; n<dirsPosibles ; n++){
                        		dir = direcciones[n];
                      	  		incrementoSegunDir(&dirFil, &dirCol, dir);
														
	              	 #define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols)
			
				if( !FDERANGO(i+dirFil, j+dirCol) ){
					ady.fil=i+dirFil;
					ady.col=j+dirCol;
					if(! meCapturan(tablero, ady, ocupante)) 
						hayMovimientos=1; /* Existe un movimiento en el que no se ve amenazado en la jugada posterior */
				}
				}
			}	
			#undef FDERANGO		
	  
		}			
	}

	if(hayMovimientos){
		if(!hayBlancos)
			estado = GANADOR_NEGRO;
		else if(!hayNegros)
			estado = GANADOR_BLANCO;
		else
			estado = SEGUIR;
	}
	else
		estado = EMPATE;


	return estado;
}


int main(void){
	
	enum tOpcion {PVP=1, PVE, CARGAR, SALIR};
	enum tOpcion opcion;
	int modo;
	tTablero tablero;
	int jugador;
	char nombre[MAX_NOM];	

	printf("\n\t\t=*=*=*=*=*=*=*=*=*==FANORONA==*=*=*=*=*=*=*=*=*=*=");
	printf("\n\t\t=*=*=*=*=*=*=*=*=FUTURAMA EDITION=*=*=*=*=*=*=*=*=\n\n");

	printf("\n\t\t=*=*=*=*=*=*=*=*=REGLAS DE JUEGO==*=*=*=*=*=*=*=*=\n");
	printf("\n\t\t    Hay bastantes reglas, Wikipedialo, ni idea\n\t\t    Ah, salvo que hay que capturar si o si\n\n");
	printf("\t\t=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=\n\n");
	printf("\tElija una opcion:\n\n");
	printf("\t\t1. Juego nuevo: dos jugadores\n");
	printf("\t\t2. **Cerrado por refacciones** Juego nuevo: un jugador (vs. computadora) \n");
	printf("\t\t3. Cargar partida de archivo\n");
	printf("\t\t4. Salir\n\n");
	do{
		opcion=getint("Ingrese un numero de opcion >"); 
	}while(opcion<1 || opcion>4);

	if(opcion == PVP || opcion == PVE){
		modo = (opcion == PVP); /* 1 para dos jugadores, 0 para juego contra la computadora*/	
		jugador=BLANCO;
		if(opcion == PVE){
			printf("\nCERRADO POR REFACCIONES. ASSERT'D!\n\n");
			return 1;
			}
		pedirDimensiones(&tablero);
		tablero=generarTablero(tablero.filas,tablero.cols);

	}
	else if(opcion == CARGAR){
		printf("Ingrese el nombre del archivo:\n   >");
		getlinea(nombre, MAX_NOM);
		tablero=cargarPartida(&modo, &jugador, nombre); /*modo cambia al correspondiente (0 o 1)*/
	        printf("\nMODO DE JUEGO: %s\n", modo?"DOS JUGADORES":"JUGADOR VS COMPUTADORA");
		printf("TURNO DEL JUGARDOR %s\n", jugador?"NEGRO":"BLANCO");
		printf("DIMENSIONES DEL TABLERO: %dx%d\n\n",tablero.filas,tablero.cols);
	}
			
	else{
		printf("\n\t\t¡Adios!\n\n");
		return 0;
	}


	jugar(tablero, modo, jugador);		

	return 0;

}

int jugar(tTablero tablero, int modo, int jugador){

	enum tDireccion dir=NULA;
	tMovimiento mov;
	char nombre[MAX_NOM];
	tFlag jugada, quiereGuardar=0, obligado=0;
	int movimiento;
	int a,b; /*TEMP*/
	int estado = SEGUIR;

	imprimirTablero(&tablero);

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
			imprimirTablero(&tablero);
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
		
			estado = estadoPostJugada(&tablero);
		
			switch(estado){
				case SEGUIR: printf("SEGUIR\n\n"); break;
				case EMPATE: printf("EMPATE\n\n"); break;
				case GANADOR_NEGRO: printf("GANA NEGRO\n\n"); break;
				case GANADOR_BLANCO: printf("GANA BLANCO\n\n"); break;
				}	


		}

		if (jugada == QUIT)
			quiereGuardar = pedirGuardar(nombre);
		if (jugada == SAVE || quiereGuardar) {
			guardarPartida(&tablero, modo, jugador, nombre);
			printf("Se ha guardado su juego con el nombre '%s'\n", nombre);
		}

	}while(jugada!=QUIT && estado==SEGUIR);
return 0;

}


/*
int puedeMover(tTablero * tablero, int jugador){
	int i,j;
	enum tDireccion direcciones[]={N,S,E,O,NE,NO,SE,SO};
	int dirsPosibles;

	for(i=0; i<tablero->filas; i++)
		for(i=0; i<tablero->cols; j++){

			#define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols)
		dirsPosibles = tablero->matriz[posicion.fil][posicion.col].tipo == FUERTE ? 8:4;  
		for(i=0; i<dirsPosibles ; i++){
                	dir = direcciones[i];
                	incrementoSegunDir(&dirFil, &dirCol, dir);
		

			#undef FDERANGO

		}		
}
*/


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
