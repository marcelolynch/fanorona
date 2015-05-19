/*			 BACK END
**	Faltaria ver como manejamos los errores
*/

#define INCORRECTO 0
#define CORRECTO 1
#define AMBIGUO 2
#include "fanorona.h"
#include <stdio.h>
#define MAXMOVS 50
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

enum tCaptura hayComida (char jugador, tTablero *tablero, tCoordenada origen, enum tDireccion direccion) { /* las coordenada podrian ser punteros */
        /*Devuelve el tipo de captura posible segun el movimiento que quiera relizar el usuario (direccion del movimiento desde de casilla origen
	** si no encuentra, devuelve NINGUNO. Si la casilla a la que se quiere mover esta ocupada, devuelve NINGUNO (no devuelve error)
	** esto es porque si se me llama de validarMovimiento() esto ya esta validado, y si se me llama de paika() me interesa solo si se puede o no hacer el movimiento */
	int fdA, fdW, cdA, cdW;
	int fueraDeRangoA, fueraDeRangoW;
	char fo = origen.fil;
        char co=origen.col;
        char enemigo = !jugador;
	enum tCaptura captura;

        signed char dirFil, dirCol;
        tFlag hayAppr=0, hayWithdr=0;

	switch(direccion){
		case N:	dirFil=-1;
			dirCol=0;
		break;
		case S:	dirFil=1;
			dirCol=0;
		break;
		case E:	dirFil=0;
			dirCol=1;
		break;
		case O:	dirFil=0;
			dirCol=-1;
		break;
		case NE: dirFil=-1;
			 dirCol=1;
		break;
		case NO: dirFil=-1;
			 dirCol=0;
		break;
		case SE: dirFil=1;
			 dirCol=1;
		break;
		case SO: dirFil=1;
			 dirCol=-1;
		break;
	}
	
	fdA=fo+2*dirFil; /*Fila de la casilla a capturar por approach */
	cdA=co+2*dirCol; /*Columna*/
	fdW=fo-dirFil; /*Fila de la casilla a capturar po widthdraw */
	cdW=co-dirFil; /*Columna*/

	/*Reviso si los casilleros que tienen las fichas a capturar existen, y si la casilla a moverse esta vacia. Si no, el movimiento es invalido */ 
	fueraDeRangoA = (tablero->matriz[fo+dirFil][co+dirCol].ocupante != VACIO || fdA < 0 || cdA < 0 || fdA >= tablero->filas || fdA >= tablero->cols);
	fueraDeRangoW = (tablero->matriz[fo+dirFil][co+dirCol].ocupante != VACIO || fdW < 0 || cdW < 0 || fdW >= tablero->filas || fdW >= tablero->cols);
	
	if (!fueraDeRangoA && tablero->matriz[fdA][cdA].ocupante== enemigo)
                /*El elementro de la matriz corresponde a la casilla a capturar por approach*/
		hayAppr = 1;
        if (!fueraDeRangoW && tablero->matriz[fdW][cdW].ocupante == enemigo)
                /*El elementro de la matriz corresponde a la casilla a capturar por withdraw*/
                hayWithdr = 1;

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
	int i, j, c;
	tCoordenada origen;
	for(i=0, c=0; i<tablero->filas ; i++)
		for(j=0; j<tablero->cols ; j++, c=0){
			if(tablero->matriz[i][j].ocupante == jugador){
				origen.fil=i;
				origen.col=j;
				if(hayComida(jugador, tablero, origen, N)!=NINGUNO
				|| hayComida(jugador, tablero, origen, S)!=NINGUNO
				|| hayComida(jugador, tablero, origen, E)!=NINGUNO
				|| hayComida(jugador, tablero, origen, O)!=NINGUNO)
					return 0; /* Aprovecho laziness */
				else if(tablero->matriz[i][j].tipo == FUERTE
				&& (hayComida(jugador, tablero, origen, NE)!=NINGUNO
				|| hayComida(jugador, tablero, origen, NO)!=NINGUNO
				|| hayComida(jugador, tablero, origen, SE)!=NINGUNO
				|| hayComida(jugador, tablero, origen, SO)!=NINGUNO))
					return 0;
			}
		}

	return 1; /*Estamos en situacion de paika*/
}

int validarMovimiento(char jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia, tFlag limpiar){
/*TODAVIA NO TERMINE DE ESCRIBIR ESTO. CREO QUE FALTAN COSAS*/ /* me parece que falta ver si se va de los limites del tablero 
							       ** también creo que debería devolver qué tipo de captura es, así
							       ** en caso de que sea ambiguo, ya preguntas si se trata de A o W*/	
/*HAY MUCHOS CASOS DE ERROR, ENTONCES HAY MUCHOS RETURNS. CORREGIR? */ /* habría que preguntar, para mi no hay que corregir --Tomás */

	int jugada, aux;

	int fo=movimiento.coordOrig.fil;
	int co=movimiento.coordOrig.col;
	int fd=movimiento.coordDest.fil;
	int cd=movimiento.coordDest.col;
	
	enum tDireccion direccionMov;
	int i;
	
	static tCasilla * casillasVisitadas[MAXMOVS]; 

	if (limpiar) {
		for(i=0; casillasVisitadas[i]!=NULL ;i++)	/*En casillasVisitadas se guardara la direccion de las casillas visitadas. Cada ves que cambia el jugador, hay que limpiarlo */
			casillasVisitadas[i]=NULL;
	}

	if(fd < 0 || fo < 0 || co < 0 || cd < 0 || fo >= tablero->filas || fd >= tablero->filas || co >= tablero->cols || cd >= tablero->cols)
		return ERR_MOV_RANGO; /*Fuera de limites del tablero*/

	if(jugador != tablero->matriz[fo][co].ocupante)
		return ERR_MOV_ORIG; /* No puede mover la ficha porque no es suya */

	if(tablero->matriz[fd][cd].ocupante != VACIO)
		return ERR_MOV_DEST; /* No puede mover porque la casilla no esta vacia */

	for(i=0; casillasVisitadas[i] != NULL ;i++)
		if(&(tablero->matriz[fd][cd]) == casillasVisitadas[i])
			return ERR_MOV_TOC;	/*No puede moverse ahi porque ya estuvo antes en este turno */

	direccionMov = direccionDestino (movimiento.coordOrig, movimiento.coordDest);

	if(direccionMov == ERROR)
		return ERR_MOV_NO_ADY; /*No es una direccion valida*/

	if(tablero->matriz[fd][cd].tipo == DEBIL && (direccionMov > O)) /* NE, NO, SE, SO */  
		return ERR_MOV_DEBIL;

	if(direccionMov == *direccionPrevia)
		return ERR_MOV_DIR;		/*No puede moverse en la misma direccion en la que venia moviendose */	

	if((aux=hayComida(jugador, tablero, movimiento.coordOrig, direccionMov)) != NINGUNO || paika(jugador, tablero))
		/*Solamente chequeo la situacion de Paika si no selecciono un movimiento donde pueda comer */
		jugada=aux;
	else
		return ERR_MOV_PAIKA;
	
	/*Si llegue hasta aca, no hay ningun error; actualizo el estado luego de que se efectue el movimiento*/
	casillasVisitadas[i]=&(tablero->matriz[fo][co]); /* i ya esta al final de casillasVisitadas (el primer NULL). Me estoy yendo de la casilla, la agrego como tocada*/
	*direccionPrevia = direccionMov;

	if(jugada==AMBOS && movimiento.tipoMov!=NINGUNO)
		/*Si el jugador puede hacer ambas cosas pero ya eligio*/
		jugada=movimiento.tipoMov;

	return jugada;
}
int main(void){
	 return 0;
}
