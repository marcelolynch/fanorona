#include "fanorona.h"
#include "getnum.h"

int jugar2(tTablero tablero, int modo, int jugador);
void pedirDimensiones(tTablero * tablero);
int leerLinea(char str[], int dim);
tFlag pedirJugada(tMovimiento *mov, char *nombre);
static tFlag validarFormato (char str[], int dim, tMovimiento *mov, char *nombre);
static const char *salteaEspacios (const char str[], int *cantEspacios);
tFlag validarMovFormato (const char str[], tMovimiento *mov);
enum tCaptura leerCaptura (const char str[]);
static const char *leerCoord (const char str[], tCoordenada *coord);
int validarFmtNombre(char destino[], const char origen[], int longOrigen); 
tFlag leerSN(void);
void pedirNombre (char nombre[]);
void imprimirTablero ( tTablero * tablero );
void pedirCadena (tMovimiento *mov);
void imprimirError(tFlag error);
enum tCaptura pedirCaptura (void);


int main(void){
	
	
	enum tOpcion opcion;
	int modo;
	tTablero tablero;
	int jugador;
	char nombre[MAX_NOM];	
	tFlag ganador;
	
	srand(time(0));

	printf("\n\t\t=*=*=*=*=*=*=*=*=*==FANORONA==*=*=*=*=*=*=*=*=*=*=");
	printf("\n\t\t=*=*=*=*=*=*=*=*=FUTURAMA EDITION=*=*=*=*=*=*=*=*=\n\n");

	printf("\n\t\t=*=*=*=*=*=*=*=*=REGLAS DE JUEGO==*=*=*=*=*=*=*=*=\n");
	printf("\n\t\t    Hay bastantes reglas, Wikipedialo, ni idea\n\t\t    Ah, salvo que hay que capturar si o si\n\n\t\tEl formato de movida es\"M [Fo,Co][Fd,Cd][w]\"\n\t\to \"M [Fo,Co][Fd,Cd][a]\", \n\t\t siendo Fo,Co Fd,Cd las coordenadas de origen y destino, \n\t\ty w/a el tipo de movida (WITHDRAWAL o APPROACH)\n\n");
	printf("\t\t=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=\n\n");
	printf("\tElija una opcion:\n\n");
	printf("\t\t1. Juego nuevo: vs PC\n");
	printf("\t\t2. Juego nuevo: dos jugadores\n");
	printf("\t\t3. Cargar partida de archivo\n");
	printf("\t\t4. Salir\n\n");
	do{
		opcion=getint("Ingrese un numero de opcion > ") - 1; 
	}while(opcion<0 || opcion>3);

	if(opcion == PVP || opcion == PVE){
		modo = opcion; /* )1 para dos jugadores, 0 para juego contra la computadora*/	
		jugador=BLANCO;
		pedirDimensiones(&tablero);
		tablero=generarTablero(tablero.filas,tablero.cols);
	}
	else if(opcion == CARGAR){
		printf("Ingrese el nombre del archivo:\n > ");
		leerLinea(nombre, MAX_NOM);
		tablero=cargarPartida(&modo, &jugador, nombre); /*modo cambia al correspondiente (0 o 1)*/
	        if(tablero.matriz == NULL){
			printf("\nError al intentar abrir el archivo \"%s\". Abortando...\n\n", nombre);
			return 1;
		}
		printf("\nMODO DE JUEGO: %s\n", modo?"DOS JUGADORES":"JUGADOR VS COMPUTADORA");
		printf("TURNO DEL JUGARDOR %s\n", jugador?"NEGRO":"BLANCO");
		printf("DIMENSIONES DEL TABLERO: %dx%d\n\n",tablero.filas,tablero.cols);
	}
			
	else{
		printf("\n\t\t¡Adios!\n\n");
		return 0;
	}


	if(modo==PVE){
		/*Inicializo el tablero auxiliar para los UNDO*/
		tablero.tableroAuxiliar = generarMatrizTablero(tablero.filas, tablero.cols);	
		printf("INI\n");
	}
	
	ganador = jugar2(tablero, modo, jugador);		

	switch(ganador) {
		case GANADOR_BLANCO: printf("GANA BLANCO\n\n"); break;
		case EMPATE: printf("\t\tEMPATE\n\n"); break;
		case GANADOR_NEGRO: printf("GANA NEGRO\n\n"); break;
	}

	return 0;

}

int jugar2(tTablero tablero, int modo, int jugador){

	tMovimiento mov;
	char nombre[MAX_NOM];
	tFlag jugada=START, quiereGuardar=0, quiereCambiar, hayGanador=0, calcularEstado=1;
	int captura;

	imprimirTablero(&tablero);
		

	while (!hayGanador && jugada != QUIT) {
		if (calcularEstado) {
			hayGanador = estadoJuego(&tablero, jugador);
			calcularEstado = 0;
		}

		if (!hayGanador) {

			if (puedeEncadenar() && (jugador == BLANCO || modo == PVP)) /* si hay cadena y no es la computadora */
				pedirCadena(&mov);
			else if (jugador == BLANCO || modo == PVP) 	/* si no es la computadora */
				jugada = pedirJugada(&mov, nombre);
			else{
				/*Mueve la computadora */
				if(calcularMovCompu(&mov, &tablero) != 0){
					imprimirError(ERR_MEM_COMPU);
					exit(1);
				}
			}
			if (jugada == MOV) {
			
				captura = mover(jugador, modo, &tablero, &mov);
			
				if (captura == AMBOS) {
					/*Hay que pedirle que especifique*/
					mov.tipoMov = pedirCaptura();
					captura = mover (jugador, modo, &tablero, &mov);
				}
				if (captura >= 0) { /* si el movimiento fue válido */
					imprimirTablero(&tablero);
					if (!puedeEncadenar()) { /* cambiamos de turno */
						cambiarTurno (&jugador, &tablero);
						calcularEstado=1;
						printf("Cambio!\nLe toca al jugador %s\n", jugador ? "negro" : "blanco");
					}
				}
				else
					imprimirError(captura);
			}
			else if (jugada == UNDO) {
				if (modo == PVE) {
					if(undo(&tablero) != 0){
						imprimirTablero(&tablero);
						calcularEstado = 1; /*Tiene que volver a chequear el tablero */
					}
					else
						imprimirError(ERR_UNDO_DOBLE);
				}
				else
					imprimirError(ERR_UNDO);
				
				/* aca va lo del error del undo */
			}

			else if (jugada == QUIT) {
				printf("Desea guardar su juego antes de salir?\n");
				quiereGuardar = leerSN();
				if (quiereGuardar)
					pedirNombre(nombre);
			}

			if (jugada == SAVE || quiereGuardar) {
				do {
					printf("Desea guardar su juego con el nombre '%s'?\n", nombre);
					quiereCambiar = !leerSN();
					if (quiereCambiar)
						pedirNombre(nombre);
				} while (quiereCambiar);
				if (guardarPartida(&tablero, modo, jugador, nombre) != ERROR)
					printf("Se ha guardado su juego con el nombre '%s'\n", nombre);
				else
					imprimirError(ERR_SAVE);
			}
		}
	}

	return hayGanador; /* si se salió por QUIT, hayGanador vale 0 */
}

void pedirDimensiones(tTablero * tablero){	
tFlag hayError;
	int decision;
	int cantfils, cantcols;

	do {
		hayError = 0;
		printf("Por favor, ingrese las dimensiones del tablero con el que desea jugar:\n");

		do {
			if (hayError)
				printf("No ingresó dimensiones correctas. Ingrese nuevamente:\n");
			cantfils= getint("Cantidad de Filas (impar entre %d y %d) > ", MIN_DIM, MAX_DIM); 
			cantcols= getint("Cantidad de Columnas (impar entre %d y %d, mayor o igual que las filas) > ", MIN_DIM, MAX_DIM);
			putchar('\n');
			hayError = 1;
		} while ( !ES_IMPAR(cantfils) || !ES_IMPAR(cantcols) || !ES_DIM_VALIDA(cantfils, cantcols) || cantfils > cantcols);
		/* OJO si se ingresan mal las filas pide columnas igual. */

		printf("Las dimensiones del tablero serán: %d x %d\n\n", cantfils,cantcols);
		printf("¿Desea continuar?\nIngrese S si es así o N para ingresar nuevas dimensiones.\n");

		decision = leerSN();
	} while (decision != 1); /* se desean ingresar nuevas dimensiones */

	tablero->filas=cantfils;
	tablero->cols=cantcols;
	return;
}

int leerLinea(char str[], int dim){
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
		printf(" > ");
		n = leerLinea(str, STR_DIM);
		jugada = validarFormato (str, n, mov, nombre);
		imprimirError(jugada); /* solo imprime en casos de error */
	} while (jugada < 0); /* hay algún tipo de error en el formato */

	return jugada;
}

static tFlag validarFormato (char str[], int dim, tMovimiento *mov, char *nombre) {
	tFlag jugada = ERR_FMT;

	if (str[0] != 'M') /* ignora si la primer letra es mayúscula o minúscula excepto si es M, pues el movimiento se llama con M */
		str[0] = tolower(str[0]);

	if (dim == MIN_STR && str[0] != 'M') {
		if (strcmp(str, "quit") == 0)
			jugada = QUIT;
		else if (strcmp(str, "undo") == 0)
			jugada = UNDO;
	}

	else if (dim > LONG_SAVE && strncmp(str, "save ", LONG_SAVE) == 0)
		jugada = validarFmtNombre (nombre, str+LONG_SAVE, dim-LONG_SAVE); /*str+LONG_SAVE es la dirección en donde comienza el nombre
										  ** y dim-LONG_SAVE es la longitud de dicho nombre */

	else if (dim <= MAX_MOV) /* es un posible movimiento */
		jugada = validarMovFormato (str, mov);

	return jugada;
}

static const char *salteaEspacios (const char str[], int *cantEspacios) {
	*cantEspacios = 0;
	while (isspace(*str)) {
		str++;
		(*cantEspacios)++;
	}
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

static const char *leerCoord (const char str[], tCoordenada *coord) {
	/* devuelve la dirección siguiente al último carácter leído o NULL en caso de error. Modifica coord */
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

int validarFmtNombre(char destino[], const char origen[], int longOrigen) {
	const char *nuevoNombre;
	int cantEspacios;
	int longNombre;

	nuevoNombre = salteaEspacios(origen, &cantEspacios); /* nuevoNombre apunta el primer caracter distinto de espacio */

	longNombre = longOrigen - cantEspacios; /* longitud del nombre; los espacios iniciales se desestiman */

	if (longNombre >= 1 && longNombre <= MAX_NOM) {
		strcpy(destino, nuevoNombre);
		return SAVE;
	}

	if (longNombre == 0)
		return ERR_FMT_SAVE1; /* se ingresaron solo espacios como nombre */
	return ERR_FMT_SAVE2; /* la longitud del nombre es mayor de lo permitida */
}

tFlag leerSN(void) {
	char c;
	char str[3];

	do {
		printf("Ingrese S o N > ");

		leerLinea(str, 3); /* str de dimensión 3 así si se ingresa más de S o N str[1] será distinto de '\0' */
		c = toupper(str[0]);
	} while( (c != 'S' && c!= 'N') || str[1] != '\0');
	
	return c == 'S';
}

void pedirNombre (char nombre[]) {
	char str[STR_DIM];
	int n;
	tFlag esValido = 1;

	printf("Escriba el nombre con el que quiere guardar su juego. (Max %d caracteres)\n", MAX_NOM);

	do {
		printf(" > ");
		imprimirError(esValido); /* en una primera instancia no imprime nada, pues esValido es mayor o igual a 0*/
		n = leerLinea(str, STR_DIM);
		esValido = validarFmtNombre (nombre, str, n);
	} while (esValido < 0);

	return;
}

void imprimirTablero ( tTablero * tablero ){
	int i, j;
	static char idColor[]={'B', 'N', 'O'}; /*BLANCO, NEGRO, VACIO*/

	for(i=-1; i<tablero->filas; i++)
		{	putchar('\n');
			if(i<0)
				printf("\t\t\t");
			else
				printf("\t\t\t%d\t",i<0?' ':i+1);
			for(j=0; j<tablero->cols; j++)
			{	if(i==-1)
				{	if(j==0)
						putchar('\t');
					printf("%-4d",j+1);

					if( j== tablero->cols-1)
						putchar('\n');
				}
				else
				{	if (tablero->matriz[i][j].tipo==DEBIL)
						printf("%-4c", tolower( idColor[ tablero->matriz[i][j].ocupante ] )); /*BLANCO=0, NEGRO=1, VACIO=2*/
					else 
						printf("%-4c", idColor[ tablero->matriz[i][j].ocupante ]);
				}
			}
		}
		putchar('\n');
	return;		
}	

void pedirCadena (tMovimiento *mov) {
	tFlag esValido = 1;
	char str[STR_DIM2]; 
	char nuevoStr[15]; /* tamaño suficiente para evaluar si el usuario introdujo de más */
	int fo, co;
	int n;

	fo = ++(mov->coordOrig.fil); /* sumamos 1 a las coordenadas, pues leerCoord les resta 1 */
	co = ++(mov->coordOrig.col);

	printf("Puede encadenar una movimiento!\n");
	printf("Ingrese solo la coordenada de la casilla a la que desea moverse y el tipo de captura si es necesario\n");
	printf("Se imprimira su nueva casilla de origen.\n");

	do {
		imprimirError(esValido); /* en una primer instancia no imprimirá nada, pues esValido es mayor a 0 */

		printf("> M [%d,%d]", fo, co);

		n = leerLinea(nuevoStr, 15);

		if (n >= MIN_STR2 && n <= MAX_STR2) {
			sprintf(str, "M [%d,%d]%s", fo, co, nuevoStr);
			esValido = validarMovFormato (str, mov); /* devuelve menor a 0 en caso de error */
		}
		else
			esValido = ERR_FMT_MOV1;

	} while (esValido < 0); /* si es menor a cero es error */

	return; 
}

void imprimirError(tFlag error) {
	if (error >= 0) /* no hay error */
		return;

	switch (error) {
	case ERR_FMT: 
		printf("\aError: no se introdujo un tipo de jugada válido.\n"); 
		break;
	case ERR_FMT_SAVE1: 
		printf("\aError: no se introdujo un nombre válido; debe contener al menos un carácter distinto de espacio.\n"); 
		break;
	case ERR_FMT_SAVE2:
		printf("\aError: se introdujo un nombre más largo de lo permitido.\n");
		break;
	case ERR_FMT_MOV1: 
		printf("\aError: no se introdujo un movimiento con el formato adecuado o algúna coordenada es mayor de lo permitido.\n");
		printf("El formato debe ser: M [Fo,Co][Fd,Cd][a/w] siendo la especificación de captura opcional en casos donde ésta no sea ambigua.\n");
		break;
	case ERR_FMT_MOV2: 
		printf("\aError: no se especificó la captura correctamente.\n");
		printf("La misma debe añadirse como [a] para approach o [w] para withdrawal inmediatamente seguido de la coordenada destino.\n");
		break;
	case ERR_MOV_ORIG:
		printf("\aError: la casilla elegida no contiene una ficha de su color.\n");
		break;
	case ERR_MOV_DEST:
		printf("\aError: la casilla a la que quiere moverse no se encuentra vacía.\n");
		break;
	case ERR_MOV_TOC:
		printf("\aError: no puede moverse a una casilla ya visitada en un mismo turno.\n");
		break;
	case ERR_MOV_DIR:
		printf("\aError: no puede moverse en la misma dirección del movimiento anterior.\n");
		break;
	case ERR_MOV_PAIKA:
		printf("\aError: no puede mover una ficha que no puede realizar una captura cuando existen otras que sí pueden.\n");
		break;	
	case ERR_MOV_RANGO:
		printf("\aError: no puede moverse fuera del tablero\n");
		break;
	case ERR_MOV_NO_ADY:
		printf("\aError: debe moverse a una casilla adyacente\n");
		break;
	case ERR_MOV_DEBIL:
		printf("\aError: desde una casilla debil solo puede moverse arriba, abajo, y a ambos costados\n");
		break;
	case ERR_UNDO:
		printf("\aError: no puede realizar UNDO si el juego es entre dos jugadores\n");
		break;
	case ERR_UNDO_DOBLE:
		printf("\aError: no puede realizar UNDO dos veces seguidas o en el primer turno (¡no hay nada para deshacer!)\n");
		break;
	case ERR_MEM_COMPU:
		printf("\aError fatal: no hay memoria suficiente para continuar con el juego. Abortando\n");
		break;
	case ERR_SAVE:
		printf("\aError: no se ha podido guardar su partida\n");
		break;
	default:
		printf("\aError desconocido\n");
		break;
	}
}

enum tCaptura pedirCaptura (void) {
	char str[5];
	int tipoCaptura;

	printf("Especifique algún tipo de captura, pues el movimiento que quiere realizar admite tanto captura por acercamiento como por alejamiento.\n");

	do {
		printf("Ingrese [w] o [a] > ");
		leerLinea(str, 5);
		tipoCaptura = leerCaptura(str);
	} while (tipoCaptura == ERROR);
	
	return tipoCaptura;
}
