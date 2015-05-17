/*			 BACK END
**	Faltaria ver como manejamos los errores
*/

#define INCORRECTO 0
#define CORRECTO 1
#define AMBIGUO 2

#include "fanorona.h"

static enum tDireccion direccionDestino(tCoordenada origen, tCoordenada destino){
	enum tDireccion direccion;
	char fo=origen.fila;
	char co=origen.col;
	char fd=destino.fila;
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

	return direccion;


}

int tJugada validarMovimiento(char jugador, tTablero tablero,tMovimiento movimiento ,enum tDireccion * direccionPrevia, tFlag limpiar){
/*TODAVIA NO TERMINE DE ESCRIBIR ESTO. CREO QUE FALTAN COSAS*/	
/*HAY MUCHOS CASOS DE ERROR, ENTONCES HAY MUCHOS RETURNS. CORREGIR? */ /* habría que preguntar, para mi no hay que corregir --Tomás */

	int jugada;

	char fo=movimiento.coordOrig.fila;
	char co=movimiento.coordOrig.col;
	char fd=movimiento.coordDest.fila;
	char cd=movimiento.coordDest.col;
	
	enum tDireccion direccionMov;
	int i;
	
	static * tCasilla casillasVisitadas[MAXMOVS]; /* creo q es mejor que sea tCoordenada y vamos guardando las coordOrigen --Tomás*/

	for(i=0; casillasVisitadas[i]!=NULL ;i++)		/*En casillasVisitadas se guardara la direccion de las casillas visitadas. Cada ves que cambia el jugador, hay que limpiarlo */
		casillasVisitadas[i]=NULL;
		

	if(jugador != tablero[fo][co].ocupante)
		return INCORRECTO; /* No puede mover la ficha porque no es suya */

	if(tablero[fd][cd].ocupante != VACIO)
		return INCORRECTO; /* No puede mover porque la casilla no esta vacia */

	for(i=0; casillasVisitadas[i] != NULL ;i++)
		if(&tablero[fd][cd] == casillasTocadas[i]) /* podríamos poner movimiento.coordDest == casillasTocadas[i] --Tomás */
			return INCORRECTO;	/*No puede moverse ahi porque ya estuvo antes en este turno */

	direccionMov=direccionDestino(movimiento.coordOrig, movimiento.coordDest);

	if(direccionMov == *direccionPrevia)
		return INCORRECTO;		/*No puede moverse en la misma direccion en la que venia moviendose */

	if(hayComida(destino) || paika())
		/*Solamente chequeo la situacion de Paika si no selecciono un movimiento donde pueda comer */
		jugada=CORRECTO;
	else
		return INCORRECTO;
		
	/*Si llegue hasta aca, no hay ningun error; actualizo el estado luego de que se efectue el movimiento*/
	casillasVisitadas[i]=&tablero[fo][co]; /* i ya esta al final de casillasVisitadas (el primer NULL). Me estoy yendo de la casilla, la agrego como tocada*/
	*direccionPrevia = direccionMov;
	
	return jugada;
}
