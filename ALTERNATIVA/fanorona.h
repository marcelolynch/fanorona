#ifndef __fanorona_header

#define __fanorona_header

#include <stdlib.h>
#include <stdio.h>

/*============================================================================*/
/* 
** MODOS DE JUEGO 
** Debe especificarse un modo de juego al comenzar la partida
*/

#define PVE 0 /* JUGADOR vs COMPUTADORA */
#define PVP 1 /* DOS JUGADORES */

/*============================================================================*/
/*
** CODIGOS DE ERROR
** Los codigos de error empiezan desde -100 y <<siempre>> seran negativos.
** Son devueltos por mover e indican algun error en la jugada que se paso
** excepto ERR_SAVE que es devuelto por guardarPartida en caso de error
*/

#define ERR_MOV_ORIG 	-100 	/* La casilla elegida no es del color correcto*/
#define	ERR_MOV_DEST 	-99 	/* La casilla destino esta ocupada */
#define	ERR_MOV_TOC 	-98	/* La casilla destino ya fue ocupada en la cadena actual*/
#define	ERR_MOV_DIR 	-97	/* La direccion de movimiento no cambio en movidas sucecivas */
#define	ERR_MOV_PAIKA 	-96	/* Quiere mover una ficha sin capturar habiendo otras que pueden hacerlo*/
#define	ERR_MOV_RANGO 	-95 	/* Quiere moverse fuera del tablero */
#define	ERR_MOV_NO_ADY	-94 	/* Quiere mover a una casilla no adyacente */
#define	ERR_MOV_DEBIL 	-93 	/* Quiere mover en diagonal desde una casilla debil */
#define	ERR_MOV_CAD 	-92	/* En el medio de una cadena, quiere mover otra ficha */
#define	ERR_UNDO	-91	/* Quiere realizar UNDO en un juego de dos jugadores */
#define	ERR_UNDO_DOBLE	-90	/* Quiere realizar un segundo undo consecutivo */

#define	ERR_MEM_COMPU	-89	/* No hay memoria suficiente para continuar */
#define ERR_PARAMS	-88	/* Los parametros pasados no se corresponden con lo que recibe la funcion*/
#define	ERR_SAVE	-87	/* No se pudo guardar la partida */
#define ERROR		 -1	/* Error generico */

/* Fin codigos de error */
#define OK 1

/*=============================================================================*/

/* TIPOS DE CAPTURA
** Para especificar en la estructura tMovimiento
** se utiliza APPROACH, WITHDRAWAL, o NINGUNO si
** no esta especificado. 
** AMBIGUO es devuelto por mover() cuando la jugada es ambigua
*/

#define NINGUNO		0
#define WITHDRAWAL 	1
#define APPROACH 	2

#define AMBIGUO		3

typedef int tCaptura;


/*=============================================================================*/

/* ESTRUCTURAS PARA MOVIMIENTO
**
** La funcion mover recibira 
** las coordenadas de movimiento 
** en una estructura tMovimiento 
*/
typedef struct {
	int fil;
	int col;
} tCoordenada;

typedef struct {
	tCoordenada coordOrig;
	tCoordenada coordDest;
	tCaptura tipoMov; /* Algun TIPO DE CAPTURA */
} tMovimiento;


/* tFlag para flags puntuales */
typedef signed char tFlag;

/*============================================================================*/

/* Constantes que determinan el ocupante (ficha) de una casilla */
#define BLANCO 0
#define NEGRO  1
#define VACIO  2

/* Constantes que definen el estado del juego 
** Son los valores que devolvera estadoJuego() */
#define SEGUIR 		0
#define GANADOR_BLANCO 	1
#define GANADOR_NEGRO 	2
#define EMPATE 		3


/* Constantes que determinan el tipo de casilla */
#define FUERTE 	1 
#define DEBIL	2


#define MIN_DIM 3 /* minima dimensión de tablero (fila o columna) permitida */
#define MAX_DIM 19 /* máxima dimensión de tablero (fila o columna) permitida */

/*============================================================================*/

/* TIPO ABSTRACTO DE DATOS: tPartida */
typedef struct tJuego * tPartida;

/* FUNCIONES PARA EL TAD: */

/* Funcion generarPartida:
** Crea una nueva partida con un tablero de fils x cols.
** Si recibe el modo PVE inicialia el tablero auxiliar.
** Devuelve el tPartida correspondiente en su nombre
*/
tPartida generarPartida(int fils, int cols, int modo);


/* Funcion cargarPartida:
** Carga una partida guardada en el directorio actual con
** el nombre especificado en el string que se recibe como parametro,
** devuelve el tPartida correspondiente a la partida
*/
tPartida cargarPartida(const char * nombre);


/* Funcion eliminarPartida: 
** libera toda la memoria reservada para la partida recibida */
void eliminarPartida(tPartida partida);

/*============================================================================*/

/* Las funciones que siguen exigen que la partida que se recibe 
** ya este inicialiada, sea por generarPartida o bien por cargarPartida */

/* 1. FUNCIONES DE CONSULTA DE ESTADOS DE LA PARTIDA */

/* hayCadena: devuelve un booleano,
** verdadero si la jugada debe continuar
** con una cadena (es obligatorio hacerlo) */
tFlag hayCadena(tPartida partida);

/* jugadorActual devuelve BLANCO o NEGRO, 
** segun el jugador que este jugando en ese momento */
int jugadorActual(tPartida partida);


/* modoJuego devuelve PVE si el juego es contra la compuradora
** o PVP si es de dos jugadores*/
int modoJuego(tPartida partida);

/* consultarOcupante devuelve BLANCO, NEGRO o VACIO
** segun la ficha que ocupe la posicion [f,c] del tablero
** (f y c empiezan en 0) */
int consultarOcupante(tPartida partida, int f, int c);


/* consultarTipo devuelve FUERTE o DEBIL
** segun la casilla que este en la posicion  [f,c] del tablero
** (f y c empiezan en 0) */
int consultarTipo(tPartida partida, int f, int c);

/* ConsultarOrigenCadena devuelve la coordenada de origen 
** luego de un encadenamiento */
tCoordenada consultarOrigenCadena(tPartida partida);

/* numFilas devuelve la cantidad de filas del tablero */
int numFilas(tPartida partida);

/* numCols devuelve la cantidad de columnas del tablero*/
int numCols(tPartida partida);




/* 2. FUNCIONES PARA EL DESARROLLOS DEL JUEGO */

/*
** Funcion mover: recibe un puntero a tMovimiento con las coordenadas (indices de 0 a N-1)
** de origen de la casilla de movimiento (ficha a mover) y destino (a donde moverse).
**
** Si la jugada es incorrecta, devuelve un codigo de error con un valor menor que cero 
** (ver CODIGOS DE ERROR). 
**
** Si la jugada no es incorrecta pero admite ambos tipos de captura, la funcion retorna AMBIGUO
** y no cambia el tablero.
**
** Si la jugada es correcta, la ejecuta y actualiza el tablero
** con las nuevas casillas, y se devuelve un valor mayor a cero. 
**
** Si el movimiento permite un encadenamiento, hayCadena() devolvera verdadero despues de la llamada.
*/
int mover (tPartida partida, tMovimiento * movimiento);


/*
** Funcion calcularMovCompu: Recibe un puntero a un tMovimiento, que 
** sera rellenado con el movimiento escogido por la computadora para
** luego ser recibido por mover()
*/
int calcularMovCompu(tMovimiento * mov, const tPartida partida); 


/* Funcion estadoJuego
** Estado del juego, para llamar antes de empezar una jugada
** devuelve GANADOR_BLANCO si el jugador 1 es el ganador
** GANADOR_NEGRO si el jugador 2 es el ganador
** EMPATE si hay un empate
** SEGUIR si el juego debe continuar
*/
int estadoJuego(const tPartida partida);



/* Funcion undo:
** Si el undo corresponde, revierte el tablero principal al estado correspondiente
** y devuelve OK.
**
** Si no corresponde por ser el segundo undo seguido, devuelve ERR_UNDO_DOBLE
** Si no corresponde por ser el modo de dos jugadores, devuelve ERR_UNDO
*/
int undo(tPartida partida);



/* Funcion cambiarTurno
** A llamarse cuando hayCadena() devuelva falso, es decir, 
** cuando hay cambio de turno.
**
** Cambia el jugador y realiza la limpieza correspondiente
** al cambio de turno.
*/
void cambiarTurno (tPartida partida);



/* Funcion guardarPartida:
** Guarda la partida en el directorio actual, con el nombre
** especificado en el string nombre que se recibe como paramentro
*/
int guardarPartida(const tPartida partida, const char * nombre);


#endif
