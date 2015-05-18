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
	else
		direccion=ERROR; /* Esta queriendo ir a casilla no adyacente */
	return direccion;


}

int tJugada validarMovimiento(char jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia, tFlag limpiar){
/*TODAVIA NO TERMINE DE ESCRIBIR ESTO. CREO QUE FALTAN COSAS*/ /* me parece que falta ver si se va de los limites del tablero 
							       ** también creo que debería devolver qué tipo de captura es, así
							       ** en caso de que sea ambiguo, ya preguntas si se trata de A o W*/	
/*HAY MUCHOS CASOS DE ERROR, ENTONCES HAY MUCHOS RETURNS. CORREGIR? */ /* habría que preguntar, para mi no hay que corregir --Tomás */

	int jugada;

	char fo=movimiento.coordOrig.fila;
	char co=movimiento.coordOrig.col;
	char fd=movimiento.coordDest.fila;
	char cd=movimiento.coordDest.col;
	
	enum tDireccion direccionMov;
	int i;
	
	static * tCasilla casillasVisitadas[MAXMOVS]; 

	if (limpiar) {
		for(i=0; casillasVisitadas[i]!=NULL ;i++)	/*En casillasVisitadas se guardara la direccion de las casillas visitadas. Cada ves que cambia el jugador, hay que limpiarlo */
			casillasVisitadas[i]=NULL;
	}

	if(fd < 0 || fo < 0 || co < 0 || cd < 0 || fo >= tablero->filas || fd >= tablero->filas || co >= tablero->cols || cd >= tablero->cols)
		return INCORRECTO; /*Fuera de limites del tablero*/

	if(jugador != tablero->matriz[fo][co].ocupante)
		return INCORRECTO; /* No puede mover la ficha porque no es suya */

	if(tablero->matriz[fd][cd].ocupante != VACIO)
		return INCORRECTO; /* No puede mover porque la casilla no esta vacia */

	for(i=0; casillasVisitadas[i] != NULL ;i++)
		if(&(tablero->matriz[fd][cd]) == casillasVisitadas[i])
			return INCORRECTO;	/*No puede moverse ahi porque ya estuvo antes en este turno */

	direccionMov = direccionDestino (movimiento.coordOrig, movimiento.coordDest);

	if(direccionMov == ERROR)
		return INCORRECTO;

	if(direccionMov == *direccionPrevia)
		return INCORRECTO;		/*No puede moverse en la misma direccion en la que venia moviendose */	

	if(hayComida(destino) || paika())
		/*Solamente chequeo la situacion de Paika si no selecciono un movimiento donde pueda comer */
		jugada=CORRECTO;
	else
		return INCORRECTO;
		
	/*Si llegue hasta aca, no hay ningun error; actualizo el estado luego de que se efectue el movimiento*/
	casillasVisitadas[i]=&(tablero->matriz[fo][co]); /* i ya esta al final de casillasVisitadas (el primer NULL). Me estoy yendo de la casilla, la agrego como tocada*/
	*direccionPrevia = direccionMov;
	
	return jugada;
}
