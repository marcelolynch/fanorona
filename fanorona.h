#ifndef __fanorona_header

#define __fanorona_header

typedef struct {
	char tipo; /* debil o fuerte */
	unsigned char ocupante; /* blanco, negro o vacio*/
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

typedef signed char tFlag;

typedef struct{
	tCasilla ** matriz;
	int filas;
	int cols;
}tTablero;


/* pensaba que podiamos poner los errores que se nos viniesen ocurriendo, 
** empieza en -120 porque así comparamos si es menor a 0, ya es error.
** cambié el tFlag a signed char. Despues se puede hacer un imprimirError y listo.*/
enum tError {ERR_FMT= -120, ERR_FMT_SAVE, ERR_FMT_MOV1, ERR_FMT_MOV2, ERR_MOV_ORIG, ERR_MOV_DEST, ERR_MOV_TOC, ERR_MOV_DIR, ERR_MOV_PAIKA, ERR_MOV_RANGO, ERR_MOV_NO_ADY,ERR_MOV_DEBIL};

enum tJugada {QUIT, SAVE, UNDO, MOV};
enum tCaptura {NINGUNO=0, WITHDRAWAL, APPROACH, AMBOS}; 
enum tDireccion {N=0, S, E, O, NE, NO, SE, SO};

#define ERROR -1

#define BLANCO 0
#define NEGRO  1
#define VACIO  2

#define FUERTE 1 
#define DEBIL 2

#define MIN_DIM 3 /* minima dimensión de tablero permitida */
#define MAX_DIM 19 /* máxima dimensión de tablero permitida */

#endif
