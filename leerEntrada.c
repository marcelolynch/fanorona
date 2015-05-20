
/*		FUNCIONES DEL FRONT END 
**	Piden jugada al usuario y validan el formato
*/

/* cosas pendientes: case-sensitivity */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "fanorona.h"

//#define BORRA_BUFFER while (getchar() != '\n')
//#define STR_DIM 31 /* long del vector que se usará para guardar la entrada del usuario */
//#define LONG_SAVE 5 /* longitud del str "save " */
//#define MIN_STR 4
//#define MIN_MOV 12 /* no está en uso. borrar o revisar */
//#define MAX_MOV 19 /* no está en uso. borrar o revisar */

//#define MAX_NOMBRE (STR_DIM-LONG_SAVE-1) /* Maxima logitud para <filename>: se resta la lonngitud de "save " y -1 por el '\0' */
//#define OK 1

tFlag pedirJugada(tMovimiento *mov, char *nombre); 
static tFlag validarFormato (char str[], int dim, tMovimiento *mov, char *nombre);
tFlag validarMovFormato (const char str[], tMovimiento *mov);
enum tCaptura leerCaptura (const char str[]);
static const char *leerCoord (const char str[], tCoordenada *coord);
static const char *salteaEspacios (const char str[]); /* devuelve la dirección del primer carácter distitno de un isspace o NULL */
void imprimirMov (tMovimiento *mov); /* TEMP */
void imprimirError(tFlag error); /* TEMP: mover a un .h luego */


/* MAIN / FUNCIONES PARA TESTEO */

/* 
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
*/


void imprimirMov (tMovimiento *mov) {
	printf("Origen: [%d, %d]\nDestino: [%d, %d]\n", mov->coordOrig.fil, mov->coordOrig.col, mov->coordDest.fil, mov->coordDest.col);
	printf("Tipo Captura: ");
	switch(mov->tipoMov) {
	case NINGUNO: printf("no especificó captura\n"); break;
	case APPROACH: printf("acercamiento\n"); break;
	case WITHDRAWAL: printf("alejamiento\n"); break;
	default: printf("TOMAS HICISTE ALGO MAL\n"); break; /* LOL --Chelo */
	}
	return;
}

/*FIN FUNCIONES DE TESTEO */
int getlinea(char str[], int dim){
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
	char str[STR_DIM];
	tFlag jugada=OK;

	printf("Ingrese una jugada:\n");

	do {
		n = getlinea(str, STR_DIM);
		jugada = validarFormato (str, n, mov, nombre);
		imprimirError(jugada); /* solo imprime en casos de error */
	} while (jugada < 0); /* hay algún tipo de error en el formato */

	return jugada;
}


static tFlag validarFormato (char str[], int dim, tMovimiento *mov, char *nombre) {
	tFlag jugada = ERR_FMT;
	const char *nuevoNombre;

	if (str[0] != 'M') /* ignora si la primer letra es mayúscula o minúscula excepto si es M, pues el movimiento se llama con M */
		str[0] = tolower(str[0]);

	if (dim == MIN_STR) {
		if (strcmp(str, "quit") == 0)
			jugada = QUIT;
		else if (strcmp(str, "undo") == 0)
			jugada = UNDO;
	}

	else if (dim > LONG_SAVE && dim < STR_DIM && strncmp(str, "save ", LONG_SAVE) == 0) {

		nuevoNombre = salteaEspacios(str+LONG_SAVE); /* nuevoNombre apunta el primer carácter distinto de espacio */

		if (nuevoNombre != NULL) {
			strcpy(nombre, nuevoNombre);
			jugada = SAVE;
		}

		else
			jugada = ERR_FMT_SAVE; /* se ingresaron solo espacios como nombre */
	}

	else if (dim <= MAX_MOV) /* es un posible movimiento */
		jugada = validarMovFormato (str, mov);

	return jugada;
}

static const char *salteaEspacios (const char str[]) {
	while (isspace(*str))
		str++;
	if (*str == '\0')
		return NULL;
	return str;
}

tFlag validarMovFormato (const char str[], tMovimiento *mov) {
	const char *p;
	enum tCaptura captura;

	if (str[0] != 'M' || str[1] != ' ')
		return ERR_FMT;

	p = &str[2]; /* direccion del supuesto primer corchete '[' */

	p = leerCoord(p, &(mov->coordOrig));
	p = leerCoord(p, &(mov->coordDest));

	if (p == NULL)
		return ERR_FMT_MOV1;

	if (*p == '\0') { /* se ingresó un movimiento sin aclaración de captura */
		captura = NINGUNO;
		mov->tipoMov = captura;
	}

	else if ( (captura = leerCaptura(p)) != ERROR ) /* en esValido se guarda APPROACH WITHDRAWAL o ERROR */
		mov->tipoMov = captura;

	if (captura != ERROR)
		return MOV;

	return ERR_FMT_MOV2; /* se introdujo mal el tipo de captura unicamente */
}


enum tCaptura leerCaptura (const char str[]) {
	if (str[0] != '[' || ( tolower(str[1]) != 'w' && tolower(str[1]) != 'a' ) || str[2] != ']' || str[3] != '\0')
		return ERROR;
	return tolower(str[1]) == 'w' ? WITHDRAWAL : APPROACH;
}


/* devuelve la dirección siguiente al último carácter leído o NULL en caso de error. Modifica coord */
static const char *leerCoord (const char str[], tCoordenada *coord) {
	int num=0;
	int i, c, filAux;
	const char *p = str;
	tFlag estado=OK, esPrimerComa=1, seEscribioNum=0;

	if (p == NULL || p[0] != '[')
		return NULL;

	for (i=1; estado != ERROR && (c=p[i])!=']' && c!='\0'; i++) {
		if (isdigit(c)) {
			num = num * 10 + c - '0';
			seEscribioNum = 1;
			if (num > MAX_DIM) /* si num es mayor a la máxima dimensión de tablero permitida */
				estado = ERROR;
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
	if (estado == ERROR || c == '\0' || esPrimerComa || !seEscribioNum)
		return NULL;

	coord->fil = filAux-1; /* se resta 1 porque en las demás funciones se empieza a contar desde 0 */
	coord->col = num-1;
	p = &p[++i]; /* direccion del carácter siguiente al ']' */
	return p;
}
