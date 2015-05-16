#ifndef __fanorona_header

#define __fanorona_header

typedef struct {
	char tipo; /* debil o fuerte */
	char ocupante; /* blanco, negro o vacio*/
} tCasilla;

typedef struct {
	char fil;
	char col;
} tCoordenada;

typedef struct {
	tCoordenada coordOrig;
	tCoordenada coordDest;
	char tipoMov; /* approach, withdrawal o ninguno */
} tMovimiento;

typedef char tFlag;
typedef tCasilla ** tTablero;

enum tJugada {ERROR = -1, QUIT, SAVE, UNDO, MOV};
enum tCaptura {NINGUNO=0, WITHDRAWAL, APPROACH}; 
enum tDireccion {N=0, S, E, O, NE, NO, SE, SO};

#define BLANCO 0
#define NEGRO 1
#define VACIO 2

#endif
