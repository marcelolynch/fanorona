#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"

#define BORRA_BUFFER while (getchar() != '\n')
#define MAX_STR 26
#define LONG_SAVE 5 /* longitud del str "save " */
#define MIN_STR 4
#define MIN_MOV 12
#define MAX_MOV 19

#define MAX_NOMBRE (MAX_STR-6) /* menos 6 pues "save " ocupa 5 y el '\0' en total da 6 */
#define OK 1

tFlag pedirJugada(tMovimiento *mov, char *nombre); 
int getlinea(char str[], int dim);
static tFlag validarFormato (const char str[], int dim, tMovimiento *mov, char *nombre);
static tFlag validarMovFormato (const char str[], tMovimiento *mov);
static tFlag leerCaptura (const char str[], tMovimiento *mov);
static const char *leerCoord (const char str[], tCoordenada *coord);

void imprimirMov (tMovimiento *mov); /* TEMP */

int main(void) {
	char str[MAX_NOMBRE];
	tMovimiento mov;
	tFlag jugada;

	jugada = pedirJugada (&mov, str);

	switch(jugada) {
	case ERROR: printf("error, no es válido\n"); break;
	case QUIT: printf("quit\n"); break;
	case SAVE: printf("save\n%s\n", str); break;
	case UNDO: printf("Undo\n"); break;
	case MOV: printf("Movimiento\n"); imprimirMov(&mov); break;
	default: printf("HAY ALGO MAL\n");
	}

	return 0;
}

void imprimirMov (tMovimiento *mov) {
	printf("Origen: [%d, %d]\nDestino: [%d, %d]\n", mov->coordOrig.fil, mov->coordOrig.col, mov->coordDest.fil, mov->coordOrig.col);
	printf("Tipo Captura: ");
	switch(mov->tipoMov) {
	case NINGUNO: printf("no especificó captura\n"); break;
	case APPROACH: printf("acercamiento\n"); break;
	case WITHDRAWAL: printf("alejamiento\n"); break;
	default: printf("TOMAS HICISTE ALGO MAL\n"); break;
	}
	return;
}

int getlinea(char str[], int dim) {
	int c, i;

	for (i=0; i<dim-1 && (c=getchar())!='\n'; i++)
		str[i] = c;
	if (c != '\n')
		BORRA_BUFFER;

	str[i] = '\0';
	return i;
}

tFlag pedirJugada(tMovimiento *mov, char *nombre) {
	int n;
	char str[MAX_STR];

	printf("Ingrese una jugada:\n");  /* TEMP */
	
	n = getlinea(str, MAX_STR);

	return validarFormato (str, n, mov, nombre);
}


static tFlag validarFormato (const char str[], int dim, tMovimiento *mov, char *nombre) {
	char jugada = ERROR;

	if (dim == MIN_STR) {
		if (strcmp(str, "quit") == 0)
			jugada = QUIT;
		else if (strcmp(str, "Undo") == 0)
			jugada = UNDO;
	}

	else if (dim > LONG_SAVE && dim < MAX_STR && strncmp(str, "save ", LONG_SAVE) == 0) {
		strcpy(nombre, str+LONG_SAVE); /* OJO puede ingresar solo espacios como nombre o espacios en el nombre? */
		jugada = SAVE;
	}

	else if (dim >= MIN_MOV && dim <= MAX_MOV)
		jugada = validarMovFormato (str, mov);

	return jugada;
}

static tFlag validarMovFormato (const char str[], tMovimiento *mov) {
	int c, i;
	const char *p;
	tFlag estado = OK;

	if (str[0] != 'M' || str[1] != ' ')
		return ERROR;
	p = &str[2]; /* direccion del supuesto primer corchete '[' */
	
	p = leerCoord(p, &(mov->coordOrig));
	p = leerCoord(p, &(mov->coordDest));

	if (p != NULL && *p == '\0') /* se ingresó un movimiento sin aclaración de captura */
		mov->tipoMov = NINGUNO;
	else if (p != NULL)
		estado = leerCaptura(p, mov);

	if (p != NULL && estado == OK)
		return MOV;

	return ERROR;
}

static tFlag leerCaptura (const char str[], tMovimiento *mov) {
	int c;
	if (str[0] != '[' || ( tolower(str[1]) != 'w' && tolower(str[1]) != 'a' ) || str[2] != ']' || str[3] != '\0')
		return ERROR;
	mov->tipoMov = tolower(str[1]) == 'w' ? WITHDRAWAL : APPROACH;
	return OK;
}

/* devuelve la dirección siguiente al último carácter leído o NULL */
static const char *leerCoord (const char str[], tCoordenada *coord) {
	int num=0;
	int i, c, filAux;
	tFlag estado=OK, esPrimerComa=1, seEscribioNum=0;

	if (str == NULL || str[0] != '[')
		estado = ERROR;

	for (i=1; estado != ERROR && (c=str[i])!=']' && c!='\0'; i++) {
		if (isdigit(c)) {
			num = num * 10 + c - '0';
			seEscribioNum = 1;
		}
		else if (c == ',' && esPrimerComa) {
			filAux = num;
			num = 0;
			esPrimerComa = 0;
			seEscribioNum = 0;
			}
		else
			estado = ERROR;
	}
	if (estado != ERROR && (c == '\0' || esPrimerComa || !seEscribioNum))
		estado = ERROR;

	if (estado != ERROR) { 
		coord->fil = filAux;
		coord->col = num;
		return &str[++i]; /* direccion del carácter siguiente al ']' */
	}

	return NULL; /* es ERROR */
}
