#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "getnum.h"
#include "fanorona.h"

int jugar2(tTablero tablero, int modo, int jugador);
void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion);
void pedirDimensiones(tTablero * tablero);
tTablero generarTablero(int fils, int cols);
void imprimirTablero ( tTablero * tablero );
tFlag pedirJugada(tMovimiento *mov, char *nombre);
void liberarTodo(tCasilla ** tablero, int n);
enum tCaptura leerCaptura (const char str[]);
void pedirDimensiones(tTablero * tablero);
tFlag pedirJugada(tMovimiento *mov, char *nombre);
void pedirNombre(char nombre[]);
void actualizarTablero(tTablero * tablero, enum tDireccion direccion, tMovimiento mov);
int validarMovimiento(int jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia, tFlag hayPaika);
void imprimirError(tFlag error);
enum tCaptura pedirCaptura (void);
void pedirCadena(tMovimiento *mov);
int jugadaObligada(tTablero * tablero, int jugador, tCoordenada origen, enum tDireccion dirAnterior);
int paika(int jugador, tTablero * tablero);
int guardarPartida(tTablero * tablero, int modo, int jugador, const char * nombre);
tTablero cargarPartida(int * modo, int * jugador, const char * nombre);
int jugar(tTablero tablero, int modo, int jugador);
int meCapturan(tTablero *tablero, tCoordenada posicion, int jugador);
tFlag leerSN(void);
tCasilla ** generarMatrizTablero(int fils, int cols);
int calcularMovCompu(tMovimiento * mov, tTablero * tablero, tFlag hayPaika, tFlag hayCadena);
void limpiarTocadas(tTablero * tablero);
int estadoPostJugada(tTablero * tablero, int jugador, tFlag * hayPaika);
void copiarTablero(tTablero * tablero, tCasilla ** tableroAuxiliar);
void intercambiarTableros(tTablero * tablero, tCasilla *** tableroAuxiliar);
int mover (int jugador, int modo, tTablero * tablero, tCasilla ** tableroAuxiliar, tMovimiento * movimiento, enum tDireccion * direccionPrevia, tFlag hayPaika, tFlag * hayCadena);
void cambiarTurno (int *jugador, tTablero * tablero, enum tDireccion * direccionPrevia);

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
int estadoPostJugada(tTablero * tablero, int jugador, tFlag * hayPaika){
	int hayBlancos=0, hayNegros=0;
	int ocupante;
	int i,j,n;
	int hayMovimientos=0;
	tCoordenada origen;
	tCoordenada ady;
	enum tDireccion direcciones[]={N, S, E, O, NE, NO, SE, SO};
	int dir, dirsPosibles, dirFil, dirCol;
	int estado;

	*hayPaika = 1;

	for(i=0; i<tablero->filas ; i++)
	for(j=0; j<tablero->cols ; j++){

		if((ocupante = tablero->matriz[i][j].ocupante) != VACIO) {
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
			if(ocupante==jugador){
			origen.fil = i;
			origen.col = j;
			if(jugadaObligada(tablero, ocupante, origen, NULA)){
				*hayPaika=0;
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
					if(tablero->matriz[ady.fil][ady.col].ocupante==VACIO && !meCapturan(tablero, ady, ocupante)) 
						hayMovimientos=1; /* Existe un movimiento en el que no se ve amenazado en la jugada posterior */
				}
				}
			}	
			#undef FDERANGO		
	  		}
		}			
	}

	if(!hayBlancos)
		estado = GANADOR_NEGRO;
	else if(!hayNegros)
		estado = GANADOR_BLANCO;
	else if(!hayMovimientos)
		estado = EMPATE;
	
	else
		estado = SEGUIR;


	return estado;
}


int estadoPostJugada2(tTablero * tablero, int jugador, tFlag * hayPaika){
	int hayBlancos=0, hayNegros=0;
	int ocupante;
	int i,j,n;
	int hayMovimientos=0;
	tCoordenada origen;
	tCoordenada ady;
	enum tDireccion direcciones[]={N, S, E, O, NE, NO, SE, SO};
	int dir, dirsPosibles, dirFil, dirCol;
	int estado;

	*hayPaika=1;

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
			if(jugadaObligada(tablero, ocupante, origen, NULA)){
				hayMovimientos=1;
				if(ocupante==jugador)
					*hayPaika=0;
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
					if(tablero->matriz[ady.fil][ady.col].ocupante==VACIO && !meCapturan(tablero, ady, ocupante)) 
						hayMovimientos=1; /* Existe un movimiento en el que no se ve amenazado en la jugada posterior */
				}
				}
			}	
			#undef FDERANGO		
	  		
		}			
	}

	if(!hayBlancos)
		estado = GANADOR_NEGRO;
	else if(!hayNegros)
		estado = GANADOR_BLANCO;
	else if(!hayMovimientos)
		estado = EMPATE;
	else
		estado = SEGUIR;


	return estado;
}
int main(void){
	
	enum tOpcion opcion;
	int modo;
	tTablero tablero;
	int jugador;
	char nombre[MAX_NOM];	
	tFlag ganador;
	
	srand(time(0));

	printf("\n\t\t=*=*=*=*=*=*=*=*=*==FANORONA==*=*=*=*=*=*=*=*=*=*=");
	printf("\n\t\t=*=*=*=*=*=*=*=*=FUTURAMA EDITION=*=*=*=*=*=*=*=*=\n\n");

	printf("\n\t\t=*=*=*=*=*=*=*=*=REGLAS DE JUEGO==*=*=*=*=*=*=*=*=\n");
	printf("\n\t\t    Hay bastantes reglas, Wikipedialo, ni idea\n\t\t    Ah, salvo que hay que capturar si o si\n\n");
	printf("\t\t=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=\n\n");
	printf("\tElija una opcion:\n\n");
	printf("\t\t1. Juego nuevo: vs PC\n");
	printf("\t\t2. Juego nuevo: dos jugadores \n");
	printf("\t\t3. Cargar partida de archivo\n");
	printf("\t\t4. Salir\n\n");
	do{
		opcion=getint("Ingrese un numero de opcion >") - 1; 
	}while(opcion<0 || opcion>3);

	if(opcion == PVP || opcion == PVE){
		modo = opcion; /* )1 para dos jugadores, 0 para juego contra la computadora*/	
		jugador=BLANCO;
		pedirDimensiones(&tablero);
		tablero=generarTablero(tablero.filas,tablero.cols);

	}
	else if(opcion == CARGAR){
		printf("Ingrese el nombre del archivo:\n   >");
		getlinea(nombre, MAX_NOM);
		tablero=cargarPartida(&modo, &jugador, nombre); /*modo cambia al correspondiente (0 o 1)*/
	        if(tablero.matriz == NULL){
			printf("\nError al intentar abrir el archivo \"%s\". Abortando...\n\n", nombre);
			return 1;
		}
		printf("\nMODO DE JUEGO: %s\n", modo?"DOS JUGADORES":"JUGADOR VS COMPUTADORA");
		printf("TURNO DEL JUGARDOR %s\n", jugador?"NEGRO":"BLANCO");
		printf("DIMENSIONES DEL TABLERO: %dx%d\n\n",tablero.filas,tablero.cols);
	}
			
	else{
		printf("\n\t\t¡Adios!\n\n");
		return 0;
	}


	ganador = jugar2(tablero, modo, jugador);		

	switch(ganador) {
		case GANADOR_BLANCO: printf("GANA BLANCO\n\n"); break;
		case EMPATE: printf("EMPATE\n\n"); break;
		case GANADOR_NEGRO: printf("GANA NEGRO\n\n"); break;
	}

	printf("\n\t\tAdios!\n\n");
	return 0;

}

void copiarTablero(tTablero * tablero, tCasilla ** tableroAuxiliar){
	int i, j;
	for(i=0; i<tablero->filas ; i++)
		for(j=0 ; j<tablero->cols ; j++)
			tableroAuxiliar[i][j] = tablero->matriz[i][j];
}	

void intercambiarTableros(tTablero * tablero, tCasilla *** tableroAuxiliar){
	tCasilla ** aux;
	aux = *tableroAuxiliar;
	*tableroAuxiliar = tablero->matriz;
	tablero->matriz = aux;

}

int jugar(tTablero tablero, int modo, int jugador){

	enum tDireccion dir=NULA;
	tMovimiento mov;
	char nombre[MAX_NOM];
	tFlag jugada=START, quiereGuardar=0, hayCadena=0, quiereCambiar, hayGanador=0, calcularGanador=1, primerUndo=1;
	int captura;
	int a,b; /*TEMP*/
	int estado = SEGUIR;
	tFlag hayPaika;
	tCasilla ** tableroAuxiliar;
	
	if(modo==PVE)
		tableroAuxiliar = generarMatrizTablero(tablero.filas, tablero.cols);	
	
	imprimirTablero(&tablero);
		

	while (!hayGanador && jugada != QUIT) {
		if (calcularGanador) {
			hayGanador = estadoPostJugada(&tablero, jugador, &hayPaika);
			calcularGanador = 0;
		}

		if (!hayGanador) {

			if (hayCadena && (jugador == BLANCO || modo == PVP)) /* si hay cadena y no es la computadora */
				pedirCadena(&mov);
			else if (jugador == BLANCO || modo == PVP) 	/* si no es la computadora */
				jugada = pedirJugada(&mov, nombre);
			else{
				/*Mueve la computadora */
				jugada=MOV;
				if(calcularMovCompu(&mov, &tablero, hayPaika, hayCadena) != 0){
					imprimirError(ERR_MEM_COMPU);
					exit(1);
				}
			}
			if (jugada == MOV) {
						
				if(modo==PVE && jugador==BLANCO){
					/*Copia el tablero al auxiliar solo antes de que juegue el usuario
					** (siempre es BLANCO) y solo si el juego es vs. Computadora*/
					copiarTablero(&tablero, tableroAuxiliar);
					primerUndo=1;
				}
				
				if (jugador == BLANCO || jugador == NEGRO) {
					captura = validarMovimiento(jugador, &tablero, mov, &dir, hayPaika);
					if (captura == AMBOS) /* ambiguedad en la jugada*/
						mov.tipoMov = pedirCaptura();
					else if (captura == APPROACH || captura == WITHDRAWAL)
						mov.tipoMov = captura;
					imprimirError(captura); /* en caso de movimiento valido, no imprime nada */
				}

				if (captura >= 0) {	/* el movimiento es válido */
					actualizarTablero(&tablero, dir, mov);
					imprimirTablero(&tablero);
					hayCadena = 0;

					if (captura != NINGUNO) /* si no fue PAIKA, busca una posible cadena */
						hayCadena = jugadaObligada(&tablero, jugador, mov.coordDest, dir);

					if (hayCadena) {
						/* al haber cadena, el origen es el nuevo destino */
						mov.coordOrig.fil = mov.coordDest.fil;
						mov.coordOrig.col = mov.coordDest.col;
					}

					else {	/* cambio de turno */
						printf("Cambio!\n");
						jugador = !jugador; /* Cambia */
						printf("\nLe toca al jugador %s\n", jugador ? "negro" : "blanco");
						dir = NULA; /* Ninguna */
						calcularGanador = 1; /* calculamos un posible ganador */
						limpiarTocadas(&tablero);
					}
				}
			}

			else if (jugada == UNDO) {
				if (modo == PVE) {
					if(primerUndo){
						intercambiarTableros(&tablero, &tableroAuxiliar);
						primerUndo=0;
						}
					else
						imprimirError(ERR_UNDO_DOBLE);
				imprimirTablero(&tablero);
				}
				else
					imprimirError(ERR_UNDO);
				
				/* aca va lo del error del undo */
			}

			else if (jugada == QUIT) {
				printf("Desea guardar su juego antes de salir?\n");
				quiereGuardar = leerSN();
				if (quiereGuardar)
					pedirNombre(nombre);
			}

			if (jugada == SAVE || quiereGuardar) {
				do {
					printf("Desea guardar su juego con el nombre '%s'?\n", nombre);
					quiereCambiar = !leerSN();
					if (quiereCambiar)
						pedirNombre(nombre);
				} while (quiereCambiar);
				guardarPartida(&tablero, modo, jugador, nombre);
				printf("Se ha guardado su juego con el nombre '%s'\n", nombre);
			}
		}
	}

	return hayGanador; /* si se salió por QUIT, hayGanador vale 0 */
}

int jugar2(tTablero tablero, int modo, int jugador){

	enum tDireccion dir=NULA;
	tMovimiento mov;
	char nombre[MAX_NOM];
	tFlag jugada=START, quiereGuardar=0, hayCadena=0, quiereCambiar, hayGanador=0, calcularGanador=1, primerUndo=0;
	int captura;
	int a,b; /*TEMP*/
	int estado = SEGUIR;
	tFlag hayPaika;
	tCasilla ** tableroAuxiliar;
	
	if(modo==PVE)
		tableroAuxiliar = generarMatrizTablero(tablero.filas, tablero.cols);	
	
	imprimirTablero(&tablero);
		

	while (!hayGanador && jugada != QUIT) {
		if (calcularGanador) {
			hayGanador = estadoPostJugada(&tablero, jugador, &hayPaika);
			calcularGanador = 0;
		}

		if (!hayGanador) {

			if (hayCadena && (jugador == BLANCO || modo == PVP)) /* si hay cadena y no es la computadora */
				pedirCadena(&mov);
			else if (jugador == BLANCO || modo == PVP) 	/* si no es la computadora */
				jugada = pedirJugada(&mov, nombre);
			else{
				/*Mueve la computadora */
				if(calcularMovCompu(&mov, &tablero, hayPaika, hayCadena) != 0){
					imprimirError(ERR_MEM_COMPU);
					exit(1);
				}
			}
			if (jugada == MOV) {
				captura = mover (jugador, modo, &tablero, tableroAuxiliar, &mov, &dir, hayPaika, &hayCadena);
				if (captura == AMBOS) {
					mov.tipoMov = pedirCaptura();
					captura = mover (jugador, modo, &tablero, tableroAuxiliar, &mov, &dir, hayPaika, &hayCadena);
				}
				if (captura >= 0) { /* si el movimiento fue válido */
					imprimirTablero(&tablero);
					if (!hayCadena) { /* cambiamos de turno */
						cambiarTurno (&jugador, &tablero, &dir);
						primerUndo=1;
						calcularGanador=1;
						printf("Cambio!\nLe toca al jugador %s\n", jugador ? "negro" : "blanco");
					}
				}
				else
					imprimirError(captura);
			}
			else if (jugada == UNDO) {
				if (modo == PVE) {
					if(primerUndo){
						intercambiarTableros(&tablero, &tableroAuxiliar);
						imprimirTablero(&tablero);
						primerUndo=0;
					}
					else
						imprimirError(ERR_UNDO_DOBLE);
				}
				else
					imprimirError(ERR_UNDO);
				
				/* aca va lo del error del undo */
			}

			else if (jugada == QUIT) {
				printf("Desea guardar su juego antes de salir?\n");
				quiereGuardar = leerSN();
				if (quiereGuardar)
					pedirNombre(nombre);
			}

			if (jugada == SAVE || quiereGuardar) {
				do {
					printf("Desea guardar su juego con el nombre '%s'?\n", nombre);
					quiereCambiar = !leerSN();
					if (quiereCambiar)
						pedirNombre(nombre);
				} while (quiereCambiar);
				guardarPartida(&tablero, modo, jugador, nombre);
				printf("Se ha guardado su juego con el nombre '%s'\n", nombre);
			}
		}
	}

	return hayGanador; /* si se salió por QUIT, hayGanador vale 0 */
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
	printf("%d,%d -> %d,%d\n", mov.coordOrig.fil+1, mov.coordOrig.col+1, mov.coordDest.fil+1, mov.coordDest.col+1);
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
