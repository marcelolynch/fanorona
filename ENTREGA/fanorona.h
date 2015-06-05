#ifndef __fanorona_header

#define __fanorona_header

#include <stdio.h>
#include "getnum.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

/* 
** empieza en -120 porque así comparamos si es menor a 0, ya es error.
** imprimirError() imprime el error correspondiente. Ir actualizando-
**/ 

enum tOpcion {PVE=0, PVP, CARGAR, SALIR};

enum tError {ERR_MOV_ORIG=-100, ERR_MOV_DEST, ERR_MOV_TOC, ERR_MOV_DIR, 
	ERR_MOV_PAIKA, ERR_MOV_RANGO, ERR_MOV_NO_ADY,ERR_MOV_DEBIL, ERR_MOV_AMBIGUO, 
	ERR_UNDO, ERR_UNDO_DOBLE, ERR_MEM_COMPU, ERR_SAVE};

enum tEstado {LIBRE=0, TOCADA, ACTIVA};

enum tJugada {START=-1, QUIT, SAVE, UNDO, MOV};

enum tCaptura {NINGUNO=0, WITHDRAWAL, APPROACH, AMBOS}; 

enum tDireccion {N=1, S, E, O, NE, NO, SE, SO, NULA};

typedef struct {
	char tipo; /* debil o fuerte */
	unsigned char ocupante; /* blanco, negro o vacio*/
	int estado;
} tCasilla;

typedef struct {
	int fil;
	int col;
} tCoordenada;

typedef struct {
	tCoordenada coordOrig;
	tCoordenada coordDest;
	char tipoMov; /* approach, withdrawal o ninguno */
} tMovimiento;

typedef struct{
	tCasilla ** matriz;
	tCasilla ** tableroAuxiliar;
	int filas;
	int cols;
}tTablero;

typedef signed char tFlag;

/* Error generico: para especificos, tError*/ 
#define ERROR -1
#define OK 1

/* Constantes que determinan el ocupante de una casilla */
#define BLANCO 0
#define NEGRO  1
#define VACIO  2

/* Constantes que definen el estado del juego al inicio 
** (final de la anterior) de una jugada */
#define SEGUIR 0
#define GANADOR_BLANCO 1
#define GANADOR_NEGRO 2
#define EMPATE 3

/* Constantes que determinan el tipo de casilla*/
#define FUERTE 1 
#define DEBIL 2

#define MIN_DIM 3 /* minima dimensión de tablero permitida */
#define MAX_DIM 19 /* máxima dimensión de tablero permitida */

/*
** Funcion mover: recibe un puntero a tMovimiento con las coordenadas (indices de 0 a N-1)
** de origen de la casilla de movimiento (ficha a mover) y destino (a donde moverse).
**
** Si la jugada es incorrecta, devuelve un codigo de error con un valor menor que cero 
** (ver enumerativos tError). Si la jugada es correcta, la ejecuta y actualiza el tablero
** con las nuevas casillas, y devuelve un valor mayor a cero. 
**
** Si el movimiento permite un encadenamiento, puedeEncadenar() devolvera verdadero despues de la llamada,
** y ademas la coordenada de origen de la estructura movimiento se cambia por la que 
** debe ser en el proximo paso de la cadena (donde cayo la ficha). 
** Esto no puede desestimarse en la jugada de la computadora. 
*/
int mover (char jugador, int modo, tTablero * tablero, tMovimiento * movimiento);


/*
** Funcion calcularMovCompu: Recibe un puntero a un tMovimiento, que 
** sera rellenado con el movimiento escogido por la computadora para
** luego ser recibido por mover()
**
** Si la jugada esta en el medio de un encadenamiento, debe recibir
** en movimiento->coordOrig la coordenada de origen 
** (mover() ya la coloco ahi en la llamada anterior)
*/
int calcularMovCompu(tMovimiento * mov, const tTablero * tablero); 


/* Funcion estadoJuego
** Estado del juego, para llamar antes de empezar una jugada
** devuelve GANADOR_BLANCO si el jugador 1 es el ganador
** GANADOR_NEGRO si el jugador 2 es el ganador
** EMPATE si hay un empate
** SEGUIR si el juego debe continuar
*/
int estadoJuego(const tTablero * tablero, int jugador);



/* Funcion puedeEncadenar
** Devuelve 1 si la siguiente jugada debe ser una cadena
** (es obligatorio encadenar), o cero si no (debe cambiar el turno)
*/
tFlag puedeEncadenar(void);


/* Funcion undo:
** Si el undo corresponde, revierte el tablero principal al estado correspondiente
** y devuelve OK.
**
** Si no corresponde por ser el segundo undo seguido, devuelve ERR_UNDO_DOBLE
** Si no corresponde por ser el modo de dos jugadores, devuelve ERR_UNDO
*/
int undo(tTablero * tablero, int modo);



/* Funcion cambiarTurno
** A llamarse cuando puedeEncadenar() devuelva falso.
** Cambia el jugador y realiza la limpieza correspondiente
** al cambio de turno
*/
void cambiarTurno (int *jugador, tTablero * tablero);



/* Funcion guardarPartida:
** Guarda la partida en el directorio actual, con el nombre
** especificado en el string nombre que se recibe como paramentro
*/
int guardarPartida(const tTablero * tablero, int modo, int jugador, const char * nombre);


/* Funcion cargarPartida:
** Carga una partida guardada en el directorio actual con
** el nombre especificado en el string que se recibe como parametro,
** devuelve el tTablero correspondiente a la partida
*/
tTablero cargarPartida(int * modo, int * jugador, const char * nombre);


/* Funcion generarTablero:
** Crea un nuevo tablero de fils x cols.
** Si recibe el modo PVE inicialia el tablero auxiliar.
** Devuelve el tTablero correspondiente en su nombre
*/
tTablero generarTablero(int fils, int cols, int modo);


#endif
