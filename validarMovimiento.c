/*			 BACK END
**	Faltaria ver como manejamos los errores
*/

#include "fanorona.h"
#define INCORRECTO 0
#define CORRECTO 1
#define AMBIGUO 2
#include <stdio.h>
#define MAXMOVS 50


void incrementoSegunDir(int * dirFil, int * dirCol, enum tDireccion direccion);
int jugadaObligada(tTablero * tablero, int jugador, tCoordenada origen);


static enum tDireccion direccionDestino(tCoordenada origen, tCoordenada destino){

	enum tDireccion direccion;
	char fo=origen.fil;
	char co=origen.col;
	char fd=destino.fil;
	char cd=destino.col;
	
	if(fd==fo-1 && cd==co)
		direccion=N;
	else if(fd==fo+1 && cd==co)
		direccion=S;
	else if(fd==fo && cd==co-1)
		direccion=O;
	else if(fd==fo && cd==co+1)
		direccion=E;
	else if(fd==fo-1 && cd==co-1)
		direccion=NO;
	else if(fd==fo-1 && cd==co+1)
		direccion=NE;
	else if(fd==fo+1 && cd==co-1)
		direccion=SO;
	else if(fd==fo+1 && cd==co+1)
		direccion=SE;
	else
		direccion=ERROR; /* Esta queriendo ir a casilla no adyacente */
	return direccion;


}


int meCapturan(tTablero *tablero, tCoordenada posicion, char jugador){

	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	int dirsPosibles = tablero->matriz[posicion.fil][posicion.col].tipo == FUERTE ? 8:4;
	int i, dirFil, dirCol;
	enum tDireccion dir;
	tCoordenada adyacente, siguienteAdy;	
	int enemigo = !jugador;

	for(i=0; i<dirsPosibles ; i++){
		dir = direcciones[i];
		incrementoSegunDir(&dirFil, &dirCol, dir);
		
		/*Las casillas que pueden comerme por approach o widthraw en el proximo turno*/
		adyacente.fil = posicion.fil + dirFil;
		adyacente.col= posicion.col + dirCol;
		siguienteAdy.fil = adyacente.fil + dirFil;
		siguienteAdy.col = adyacente.col + dirCol;

        	#define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols)		
		
		if(!FDERANGO(adyacente.fil, adyacente.col) && !FDERANGO(siguienteAdy.fil, siguienteAdy.col)){

			if(tablero->matriz[adyacente.fil][adyacente.col].ocupante == enemigo
				&& tablero->matriz[siguienteAdy.fil][siguienteAdy.col].ocupante == VACIO){
					/*Me comen por WITHDRAWAL en la proxima jugada en esa direcicon*/
				printf("Si voy a %d, %d me capturan por withdrawal desde %d, %d\n",posicion.fil+1, posicion.col+1, adyacente.fil+1, adyacente.col+1);
					return 1;
			}	
			if(tablero->matriz[adyacente.fil][adyacente.col].ocupante == VACIO
				&& tablero->matriz[siguienteAdy.fil][siguienteAdy.col].ocupante == enemigo){
					/*Me comen por APPROACH en la proxima jugada, desde esa direccion*/
					printf("Si voy a %d, %d me capturan por approach desde %d, %d\n", posicion.fil+1, posicion.col+1, siguienteAdy.fil+1, siguienteAdy.col+1);
					return 1;
		}
		#undef FDERANGO
		}
	}

	printf("No me capturan desde %d, %d\n", posicion.fil, posicion.col);
	return 0; /*Si llegue hasta aca, no hay manera de que me capturen*/	

}

enum tCaptura hayComida (char jugador, tTablero *tablero, tCoordenada origen, enum tDireccion direccion) { 

	/* Devuelve el tipo de captura posible segun el movimiento que quiera realizar el usuario
	** (direccion del movimiento desde de casilla origen). Si no encuentra, devuelve NINGUNO. 
	** Si la casilla a la que se quiere mover esta ocupada, devuelve NINGUNO (no devuelve error)
 	** Esto es porque si se me llama de validarMovimiento() esto ya esta validado,
 	** y si se me llama de paika() me interesa solo si se puede o no hacer el movimiento, 
	** no si el usuario ingreso algo valido */
	int fdA, fdW, cdA, cdW;
	int fueraDeRangoA, fueraDeRangoW;
	char fo = origen.fil;
        char co=origen.col;
        char enemigo = !jugador;
	enum tCaptura captura;

        int dirFil, dirCol;
        tFlag hayAppr=0, hayWithdr=0;
	
	incrementoSegunDir(&dirFil, &dirCol, direccion);
	
	fdA=fo+2*dirFil; /*Fila de la casilla a capturar por approach */
	cdA=co+2*dirCol; /*Columna*/
	fdW=fo-dirFil; /*Fila de la casilla a capturar por withdrawal */
	cdW=co-dirCol; /*Columna*/

	/* Reviso si los casilleros que tienen las fichas a capturar existen, y si la casilla a moverse esta vacia.
	** Si no, el movimiento es invalido (NINGUNO) */ 
		
	#define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols) 
	
	fueraDeRangoA = ( FDERANGO(fdA,cdA) || FDERANGO(fo+dirFil, co+dirCol)
			|| tablero->matriz[fo+dirFil][co+dirCol].ocupante != VACIO);
	
	fueraDeRangoW = ((FDERANGO(fdW,cdW) || FDERANGO(fo+dirFil, co+dirCol))
			 || tablero->matriz[fo+dirFil][co+dirCol].ocupante != VACIO);
	#undef FDERANGO	

	if (!fueraDeRangoA && tablero->matriz[fdA][cdA].ocupante== enemigo)
                /*El elementro de la matriz corresponde a la casilla a capturar por approach*/
	       {hayAppr = 1;
		}
        if (!fueraDeRangoW && tablero->matriz[fdW][cdW].ocupante == enemigo)
                /*El elementro de la matriz corresponde a la casilla a capturar por withdrawal*/
                {hayWithdr = 1;
		 
		}
        if (hayAppr && hayWithdr)
                captura=AMBOS;
        else if (hayAppr)
                captura=APPROACH;
        else if (hayWithdr)
                captura=WITHDRAWAL;
        else
                captura=NINGUNO;

	return captura;
}


int paika(char jugador, tTablero * tablero){
	/*Todavia no esta adaptada para ser aprovechada por el modo vs. PC */
	int i, j;
	tCoordenada origen;
	for(i=0; i < tablero->filas ; i++)
		for(j=0; j<tablero->cols ; j++){
			if(tablero->matriz[i][j].ocupante == jugador){
				origen.fil=i;
				origen.col=j;
			if(hayComida(jugador, tablero, origen, N)!=NINGUNO
				|| hayComida(jugador, tablero, origen, S)!=NINGUNO
				|| hayComida(jugador, tablero, origen, E)!=NINGUNO
				|| hayComida(jugador, tablero, origen, O)!=NINGUNO)
					{printf("Hay comida desde %d, %d\n", i+1, j+1);
					return 0;} 
				else if(tablero->matriz[i][j].tipo == FUERTE
				&& (hayComida(jugador, tablero, origen, NE)!=NINGUNO
				|| hayComida(jugador, tablero, origen, NO)!=NINGUNO
				|| hayComida(jugador, tablero, origen, SE)!=NINGUNO
				|| hayComida(jugador, tablero, origen, SO)!=NINGUNO))
					{printf("Hay comida desde %d, %d\n", i+1, j+1);
				return 0;}

			}
		}

	return 1; /*Estamos en situacion de paika*/
}

int validarMovimiento(char jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia) {
	
	int jugada, aux;

	int fo=movimiento.coordOrig.fil;
	int co=movimiento.coordOrig.col;
	int fd=movimiento.coordDest.fil;
	int cd=movimiento.coordDest.col;
	
	enum tDireccion direccionMov;
	int hayPaika;

	if(fd < 0 || fo < 0 || co < 0 || cd < 0 || fo >= tablero->filas || fd >= tablero->filas || co >= tablero->cols || cd >= tablero->cols)
		return ERR_MOV_RANGO; /*Fuera de limites del tablero*/

	if(jugador != tablero->matriz[fo][co].ocupante)
		return ERR_MOV_ORIG; /* No puede mover la ficha porque no es suya */

	if(tablero->matriz[fd][cd].ocupante != VACIO)
		return ERR_MOV_DEST; /* No puede mover porque la casilla no esta vacia */

	if(tablero->matriz[fd][cd].estado==TOCADA)
		return ERR_MOV_TOC;

	direccionMov = direccionDestino(movimiento.coordOrig, movimiento.coordDest);

	if(direccionMov == ERROR)
		return ERR_MOV_NO_ADY; /*No es una direccion valida*/

	if(tablero->matriz[fd][cd].tipo == DEBIL && (direccionMov > O)) /* NE, NO, SE, SO */  
		return ERR_MOV_DEBIL;

	if(direccionMov == *direccionPrevia)
		return ERR_MOV_DIR;		/*No puede moverse en la misma direccion en la que venia moviendose */	

	if((aux=hayComida(jugador, tablero, movimiento.coordOrig, direccionMov)) != NINGUNO || (hayPaika = paika(jugador, tablero)))
		/*Solamente chequeo la situacion de Paika si no selecciono un movimiento donde pueda comer */
		jugada=aux;
	else{
		printf("aux: %d, paika: %d", aux, hayPaika);
		return ERR_MOV_PAIKA;
	}
	
	/*Si llegue hasta aca, no hay ningun error; actualizo el estado luego de que se efectue el movimiento*/
	
	tablero->matriz[fo][co].estado=TOCADA;
	
	*direccionPrevia = direccionMov;

	if(jugada==AMBOS && movimiento.tipoMov!=NINGUNO)
		/*Si el jugador puede hacer ambas cosas pero ya eligio*/
		jugada=movimiento.tipoMov;

	/* Valido si la proxima jugada es obligada */

	return jugada;
}



int jugadaObligada(tTablero * tablero, int jugador, tCoordenada origen){

	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	int dir;
	int dirFil, dirCol;
	tCoordenada destino;
	int dirsPosibles = tablero->matriz[origen.fil][origen.col].tipo == FUERTE ? 8:4; 

	for(dir=0 ; dir<dirsPosibles ; dir++){
		incrementoSegunDir(&dirFil, &dirCol, direcciones[dir]);
		destino.fil = origen.fil + dirFil;
		destino.col = origen.col + dirCol;
		
		if(destino.fil<(tablero->filas) && destino.fil>=0 && destino.col<(tablero->cols) && destino.col>=0){

		if(tablero->matriz[destino.fil][destino.col].estado != TOCADA)  /*Solo chequeo si hay comida si no visite esa casilla antes*/
			{ //printf("%d, %d no esta tocada\n", destino.fil+1, destino.col+1);
			if(hayComida(jugador, tablero, origen, direcciones[dir])!=NINGUNO)
			{//	printf("Hay comida yendo a %d, %d\n", destino.fil+1, destino.col+1);
				return 1; /*Debe capturar esa pieza la proxima jugada */}
		}
			else
			;//	printf("%d, %d esta tocada\n", destino.fil+1, destino.col+1);
		}
		}	

	return 0;
}

