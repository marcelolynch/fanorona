#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "getnum.h"
#include "fanorona.h"

void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion);
tFlag pedirJugada(tMovimiento *mov, char *nombre);
enum tCaptura leerCaptura (const char str[]);
void pedirDimensiones(tTablero * tablero);
tFlag pedirJugada(tMovimiento *mov, char *nombre);
void actualizarTablero(tTablero * tablero, enum tDireccion direccion, tMovimiento mov);
int validarMovimiento(char jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia, tFlag hayPaika);
void imprimirError(tFlag error);
enum tCaptura pedirCaptura (void);
void pedirCadena(tMovimiento *mov);
int jugadaObligada(tTablero * tablero, int jugador, tCoordenada origen, enum tDireccion dirAnterior);
int paika(char jugador, tTablero * tablero);
int guardarPartida(tTablero * tablero, int modo, int jugador, const char * nombre);
tTablero cargarPartida(int * modo, int * jugador, const char * nombre);
int jugar(tTablero tablero, int modo, int jugador);
int meCapturan(tTablero *tablero, tCoordenada posicion, char jugador);
tFlag leerSN(void);
tCasilla ** generarMatrizTablero(int fils, int cols);
int calcularMovCompu(tMovimiento * mov, tTablero * tablero, tFlag hayPaika, tFlag hayCadena);


int mover (char jugador, int modo, tTablero * tablero, tCasilla ** tableroAuxiliar, tMovimiento * movimiento, enum tDireccion * direccionPrevia, tFlag hayPaika, tFlag * hayCadena) {
	int captura;
	
	captura = validarMovimiento(jugador, tablero, *movimiento, direccionPrevia, hayPaika);

	if (captura == AMBOS) { /* si la jugada es ambigua, retorna para que el front pida la captura */
		*direccionPrevia = NULA;
		return captura;
	}
	else if (captura >= 0) { /* si no fue error */
		/*Copia el tablero al auxiliar antes en el comienzo del turno  del usuario y si juega vs Computadora*/
		if (!(*hayCadena) && modo == PVE && jugador == BLANCO)
			copiarTablero(tablero, tableroAuxiliar);
		
		movimiento->tipoMov = captura;
		actualizarTablero(tablero, *direccionPrevia, *movimiento);
		*hayCadena = 0;

		if (captura != NINGUNO) /* si no fue PAIKA, busca una posible cadena */
			*hayCadena = jugadaObligada(tablero, jugador, movimiento->coordDest, *direccionPrevia);

		if (*hayCadena) {
			/* al haber cadena, el orígen es el nuevo destino */
			movimiento->coordOrig.fil = movimiento->coordDest.fil;
			movimiento->coordOrig.col = movimiento->coordDest.col;
		}
	}

	return captura;
}

void cambiarTurno (char *jugador, tTablero * tablero, enum tDireccion * direccionPrevia) {
	*jugador = !(*jugador);
	limpiarTocadas(tablero);
	*direccionPrevia = NULA; /* Ninguna */
}
