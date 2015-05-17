/* BACK-END */

#include <fanorona.h>

enum tCaptura hayComida (char jugador, tTablero tablero, tCoordenada origen, tCoordenada destino) { /* las coordenada podrian ser punteros */
	char fo = origen.fila;
	char co=origen.col;
	char fd=destino.fila;
	char cd=destino.col;
	char enemigo = !jugador;

	signed char dirFil, dirCol;
	tFlag hayAppr=0, hayWithdr=0;

	dirFil = fd - fo; /* dirección de la fila a la cual se dirige */
	dirCol = cd - co; /* dirección de la columna a la cual se dirige */

	if (tablero[fd+dirFil][cd+dirCol].ocupante == enemigo)
		hayAppr = 1;
	if (tablero[fo-dirFil][co-dirCol].ocupante == enemigo)
		hayWithdr = 1;
	
	if (hayAppr && hayWithdr)
		return AMBOS;
	else if (hayAppr)
		return APPROACH;
	else if (hayWithdr)
		return WITHDRAWAL;
	else
		return NINGUNO;
}
