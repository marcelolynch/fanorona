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
int jugadaObligada(tTablero * tablero, int jugador, tCasilla * visitadas[], tCoordenada origen);

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

enum tCaptura hayComida (char jugador, tTablero *tablero, tCoordenada origen, enum tDireccion direccion) { 

	/* Devuelve el tipo de captura posible segun el movimiento que quiera relizar el usuario
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
	fdW=fo-dirFil; /*Fila de la casilla a capturar por withdraw */
	cdW=co-dirFil; /*Columna*/

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
		printf("HAY APPROACH\n");}
        if (!fueraDeRangoW && tablero->matriz[fdW][cdW].ocupante == enemigo)
                /*El elementro de la matriz corresponde a la casilla a capturar por withdraw*/
                {hayWithdr = 1;
			printf("HAY WIDTHDRAW\n");
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
					{printf("Hay comida en %d %d", i, j);
					return 0;} /* Aprovecho laziness */
				else if(tablero->matriz[i][j].tipo == FUERTE
				&& (hayComida(jugador, tablero, origen, NE)!=NINGUNO
				|| hayComida(jugador, tablero, origen, NO)!=NINGUNO
				|| hayComida(jugador, tablero, origen, SE)!=NINGUNO
				|| hayComida(jugador, tablero, origen, SO)!=NINGUNO))
					{printf("Hay comida en %d %d", i, j);
					return 0;}
			}
		}

	return 1; /*Estamos en situacion de paika*/
}

int validarMovimiento(char jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia, tFlag limpiar, tFlag * proxObligado){
	
	int jugada, aux;

	int fo=movimiento.coordOrig.fil;
	int co=movimiento.coordOrig.col;
	int fd=movimiento.coordDest.fil;
	int cd=movimiento.coordDest.col;
	
	enum tDireccion direccionMov;
	int i;

	static tCasilla * casillasVisitadas[MAXMOVS]; 
	
	*proxObligado=1; /*Si hay error, esta obligado a seguir moviendo*/

	if (limpiar) {
	/*En casillasVisitadas se guarda la direccion de las casillas visitadas.
	** Cada vez que cambia el jugador, hay que limpiarlo. Si continua el turno, no. */
		for(i=0; casillasVisitadas[i]!=NULL ;i++)
			casillasVisitadas[i]=NULL;
	}

	if(fd < 0 || fo < 0 || co < 0 || cd < 0 || fo >= tablero->filas || fd >= tablero->filas || co >= tablero->cols || cd >= tablero->cols)
		return ERR_MOV_RANGO; /*Fuera de limites del tablero*/

	if(jugador != tablero->matriz[fo][co].ocupante)
		return ERR_MOV_ORIG; /* No puede mover la ficha porque no es suya */

	if(tablero->matriz[fd][cd].ocupante != VACIO)
		return ERR_MOV_DEST; /* No puede mover porque la casilla no esta vacia */

	for(i=0; casillasVisitadas[i] != NULL ;i++)
	{	printf("%d %p\n", i, casillasVisitadas[i]);
		if(&(tablero->matriz[fd][cd]) == casillasVisitadas[i])
			return ERR_MOV_TOC;	}/*No puede moverse ahi porque ya estuvo antes en este turno */

	direccionMov = direccionDestino(movimiento.coordOrig, movimiento.coordDest);

	if(direccionMov == ERROR)
		return ERR_MOV_NO_ADY; /*No es una direccion valida*/

	if(tablero->matriz[fd][cd].tipo == DEBIL && (direccionMov > O)) /* NE, NO, SE, SO */  
		return ERR_MOV_DEBIL;

	if(direccionMov == *direccionPrevia)
		return ERR_MOV_DIR;		/*No puede moverse en la misma direccion en la que venia moviendose */	

	if((aux=hayComida(jugador, tablero, movimiento.coordOrig, direccionMov)) != NINGUNO || paika(jugador, tablero))
		/*Solamente chequeo la situacion de Paika si no selecciono un movimiento donde pueda comer */
		jugada=aux;
	else{
		printf("aux: %d, paika: %d", aux, paika(jugador,tablero));
		return ERR_MOV_PAIKA;
	}
	
	/*Si llegue hasta aca, no hay ningun error; actualizo el estado luego de que se efectue el movimiento*/
	
	casillasVisitadas[i]=&(tablero->matriz[fo][co]);
	printf("%p", casillasVisitadas[i]);

	tablero->matriz[fo][co].estado=TOCADA;
	/* i ya esta al final de casillasVisitadas (el primer NULL). Me estoy yendo de la casilla, la agrego como tocada*/
	
	*direccionPrevia = direccionMov;

	if(jugada==AMBOS && movimiento.tipoMov!=NINGUNO)
		/*Si el jugador puede hacer ambas cosas pero ya eligio*/
		jugada=movimiento.tipoMov;

	/* Valido si la proxima jugada es obligada */
	*proxObligado = jugadaObligada(tablero, jugador, casillasVisitadas, movimiento.coordDest);	

	return jugada;
}



int jugadaObligada(tTablero * tablero, int jugador, tCasilla * visitadas[], tCoordenada origen){
	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	int dir, i, chequear=1;
	int dirFil, dirCol;
	tCoordenada destino;
	int dirsPosibles = tablero->matriz[origen.fil][origen.col].tipo == FUERTE ? 8:4; 

	for(dir=0 ; dir<dirsPosibles ; dir++, chequear=1){
		incrementoSegunDir(&dirFil, &dirCol, direcciones[dir]);
		destino.fil = origen.fil + dirFil;
		destino.col = origen.col + dirCol;
/*
		for(i=0; visitadas[i] != NULL ; i++)
				if(&(tablero->matriz[destino.fil][destino.col]) == visitadas[i])
					chequear=0; */
		
		if(destino.fil<(tablero->filas) && destino.fil>=0 && destino.col<(tablero->cols) && destino.col>=0){

		if(tablero->matriz[destino.fil][destino.col].estado != TOCADA)  /*Solo chequeo si hay comida si no visite esa casilla antes*/
			{ printf("%d, %d no esta tocada\n", destino.fil, destino.col);
			if(hayComida(jugador, tablero, origen, direcciones[dir])!=NINGUNO)
			{	printf("Hay comida desde %d, %d\n", destino.fil, destino.col);
				return 1; /*Debe capturar esa pieza la proxima jugada */}
		}
			else
				printf("%d, %d esta tocada\n", destino.fil, destino.col);
		}
		}	

	return 0;
}

