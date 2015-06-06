#include "fanorona.h"
#include "getnum.h"
#include <ctype.h>
#include <string.h>

#define BORRA_BUFFER while (getchar() != '\n')
#define STR_DIM 41 /* long del vector que se usará para guardar la entrada del usuario */
#define LONG_SAVE 5 /* longitud del str "save " */
#define MIN_STR 4
#define MAX_NOM (STR_DIM - LONG_SAVE - 5) 
	/* Maxima logitud para <filename>: se resta la longitud de "save " y -5 por el '\0' y para
	** saber si el usuario escribió más que el límite permitido */

#define MIN_COORD 5 /* mínima longitud de string de coordenada válida que puede escribir el usuario */
#define MAX_COORD 10  /* máxima longitud de string de coordenada válida que puede escribir el usuario */

#define ES_IMPAR(a) ((a) % 2 == 1)
#define ES_DIM_VALIDA(a, b) ( (a) >= MIN_DIM && (a) <= MAX_DIM && (b) >= MIN_DIM && (b) <= MAX_DIM )

/* Tipos posibles de jugada ingresadas por el usuario. START es para inicializar */
enum tJugada {START=-1, QUIT, SAVE, UNDO, MOV};
 
/* Se distinguen de los errores arrojados por el back-end, pero se usan de la misma manera */
enum tErrorFmt {ERR_FMT=-120, /*Error de formato*/
		ERR_FMT_SAVE1, /* Error sobre el nombre de savefile */
		ERR_FMT_SAVE2, /* Error sobre longitud de nombre de savefile */
		ERR_FMT_MOV1,  /* Error de formato en el comando de movimiento */
        	ERR_FMT_MOV2}; /* Error en el formato al especificar captura */


/* jugar controla el flujo del juego y devuelve
** el ganador o 0 en caso de QUIT */
int jugar(tPartida partida);

/* FUNCIONES QUE LEEN ENTRADA */

void pedirDimensiones(int * filas, int * columnas);

/* leerLinea guarda los dim-1 primeros caracteres 
** ingresados por el usuario o hasta \n en str[]. Coloca '\0' al final.
** Borra el buffer y devuelve lo mismo que devolvería strlen.
** Es por esto que se usan vectores de tamaño mayor de lo 
** necesario a la hora de validar formatos para así poder 
** evaluar si el usuario ingresó más información de la requerida. */
int leerLinea(char str[], int dim);

/* pedirJugada llama a leerLinea y validarFmtLinea.
** En caso de error en el formato, imprime el error correspondiente 
** y sigue pidiendo jugadas hasta que una sea valida. 
** Devuelve dicha jugada. */
enum tJugada pedirJugada(tMovimiento *mov, char *nombre);

/* pedirCaptura pide al usuario ingresar [w] o [a].
** Utilizada en casos donde el movimiento sea ambiguo. */
tCaptura pedirCaptura (void);

/* pedirCadena pide al usuario las coordenadas de destino,
** imprimiendo las de origen anteriormente en caso de que 
** éste se encuentre en medio de un encadenamiento. 
** Llama a sprintf para así poder llamar a validarFmtMov 
** pasandole string con formato adecuado al de un 
** movimiento regular. */
tMovimiento pedirCadena (tPartida partida);

/* pedirNombre llama a leerLinea y validarFmtNombre.
** Sigue pidiendo un nombre mientras este sea inválido e 
** imprime el correspondiente mensaje de error.*/
void pedirNombre (char nombre[]);

/* leerSN pide al usuario ingresar S (si) o N (no).
** Devuelve 1 o 0 respectivamente. */
tFlag leerSN(void);


/* FUNCIONES QUE VALIDAN FORMATO */

/* validarFmtLinea valida si se ingresó una jugada 
** con formato valido. Llama a validarFmtMov en caso de que
** el str pueda ser un movimiento o a validarFmtNombre
** en caso de que pueda ser un nombre. Undo y quit se validan
** en validarFmtLinea directamente.
** Devuelve el tipo de jugada o el error correspondiente.
** mov y nombre se usan únicamente como parámetros de salida. */
int validarFmtLinea (char str[], int dim, tMovimiento *mov, char *nombre);

/* validarFmtMov valida que str[] tenga un formato de movimiento válido
** y guarda en mov dichos valores. Devuelve MOV o el error correspondiente. */
int validarFmtMov (const char str[], tMovimiento *mov);

/* validarFmtCoord valida que str[] sea de la forma [f,c] siendo f y c 
** números que representan una fila y una columna del tablero respectivamente.
** Devuelve la dirección al caracter siguiente al corchete que cierra 
** o NULL en caso de un error de formato o que las coordenadas no se encuentren 
** entre MIN_DIM y MAX_DIM. */
const char *validarFmtCoord (const char str[], tCoordenada *coord);

/* validarFmtCaptura valida que el string sea de la forma 
** [c] siendo c = 'w', 'W', 'a' o 'A' y que además haya un '\0'
** inmediatamente luego del ']' */
tCaptura validarFmtCaptura (const char str[]);

/* validarFmtNombre guarda en destino[] el nombre recibido en origen[] 
** y devuelve SAVE si es válido o el error correspondiente. 
** Se considera válido a todo nombre que no esté compuesto únicamente
** por espacios y que no exceda el límite de longitud de nombre permitido MAX_NOM.
** longOrigen es la longitud del str origen[] (strlen de origen). */
int validarFmtNombre(char destino[], const char origen[], int longOrigen); 

/* salteaEspacios devuelve la dirección del primer caracter distinto de espacio
** en str[] y la cantidad de espacios salteados en cantEspacios. */
const char *salteaEspacios (const char str[], int *cantEspacios);


/* FUNCIONES QUE IMPRIMEN A PANTALLA */

void imprimirTablero (const tPartida partida );
void imprimirMov (const tMovimiento * mov);
void imprimirErrorFmt(enum tErrorFmt error);
void imprimirError(int error);


/* main crea la partida según las especificaciones del usuario 
** y llama a jugar con dicha partida. */
int main(void){
	
	int opcion;
	int modo;
	tPartida partida;
	char nombre[MAX_NOM];	
	tFlag ganador;
	int filas, columnas;
	
	printf("\n\t\t=*=*=*=*=*=*=*=*=*==FANORONA==*=*=*=*=*=*=*=*=*=*=");
	printf("\n\t\t=*=*=*=*=*=*=*=*=FUTURAMA EDITION=*=*=*=*=*=*=*=*=\n\n");

	printf("\n\t\t=*=*=*=*=*=*=*==NOTAS SOBRE EL JUEGO=*=*=*=*=*=*=*=\n");
	printf("\n\t\t\t\tLa captura es obligatoria\n");
	printf("\n\t\tLas casillas fuertes se marcan con letras mayusculas\n");
	printf("\t\ty las debiles oon minuscula");
	printf("\n\n\t\tEl formato de movida es \"M [Fo,Co][Fd,Cd][w]\", o bien\n");
	printf("\t\t\"M [Fo,Co][Fd,Cd][a]\", siendo Fo,Co Fd,Cd las coordenadas\n");
	printf("\t\tde origen y destino, y w/a el tipo de movida (WITHDRAWAL \n");
	printf("\t\t o APPROACH)\n\n");
	printf("\t\t=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=\n\n");
	printf("\tElija una opcion:\n\n");
	printf("\t\t1. Juego nuevo: vs PC\n");
	printf("\t\t2. Juego nuevo: dos jugadores\n");
	printf("\t\t3. Cargar partida de archivo\n");
	printf("\t\t4. Salir\n\n");
	do{
		opcion=getint("Ingrese un numero de opcion > "); 
	}while(opcion<1 || opcion>4);

	if(opcion == 1 || opcion == 2){
		modo = opcion == 1 ? PVE : PVP; /* 1 es juego contra la computadora, 2 es dos jugadores*/	
		pedirDimensiones(&filas, &columnas);
		partida=generarPartida(filas,columnas, modo);
	
		if(partida == NULL){
			imprimirError(ERR_MEM_COMPU);
			return 1;
		}
	}
	else if(opcion == 3){
		/*Cargar partida*/
		printf("Ingrese el nombre del archivo:\n > ");
		leerLinea(nombre, MAX_NOM);
		partida=cargarPartida(nombre); /*modo cambia al correspondiente (0 o 1)*/
	        if(partida == NULL){
			printf("\nError al intentar abrir el archivo \"%s\". Abortando...\n\n", nombre);
			return 1;
		}
		printf("\nMODO DE JUEGO: %s\n", modoJuego(partida)?"DOS JUGADORES":"JUGADOR VS COMPUTADORA");
		printf("TURNO DEL JUGADOR %s\n", jugadorActual(partida)?"NEGRO":"BLANCO");
		printf("DIMENSIONES DEL TABLERO: %dx%d\n\n", numFilas(partida),numCols(partida));
	}
			
	else{
		/* Salir */
		printf("\n\t\t¡Adios!\n\n");
		return 0;
	}


	ganador = jugar(partida);		

	switch(ganador) {
		case GANADOR_BLANCO: printf("\a\n\n\t===========GANADOR: BLANCO==========\n\n"); break;
		case EMPATE: printf("\a\n\n\t===========EMPATE===========\n\n"); break;
		case GANADOR_NEGRO: printf("\a\n\n\t==========GANADOR: NEGRO===========\n\n"); break;
	}

	return 0;

}

int jugar(tPartida partida){

	tMovimiento mov;
	char nombre[MAX_NOM];
	tFlag quiereGuardar=0, quiereCambiar, quiereUndo, hayGanador=0, calcularEstado=1;
	enum tJugada jugada = START;
	int captura;

	imprimirTablero(partida);
		
	while (!hayGanador && jugada != QUIT) {
		if (calcularEstado) {
			hayGanador = estadoJuego(partida);
			calcularEstado = 0;
		}

		if (!hayGanador) {

			if ( hayCadena(partida) && ( jugadorActual(partida) == BLANCO || modoJuego(partida) == PVP)) 
				/* si hay cadena y no es la computadora */
				mov = pedirCadena(partida);

			else if (jugadorActual(partida) == BLANCO || modoJuego(partida) == PVP){ 	
				/* si no es la computadora */
				printf("\nTurno del jugador %s\n", jugadorActual(partida) ? "negro" : "blanco");
				jugada = pedirJugada(&mov, nombre);
				}
			else{
				/*Mueve la computadora */
				printf("\nMueve la computadora:");
				if(calcularMovCompu(&mov, partida) != 0){
					imprimirError(ERR_MEM_COMPU);
					exit(1);
				}
			}
			if (jugada == MOV) {
			
				captura = mover(partida, &mov);
			
				if (captura == AMBIGUO) {
					/*Hay que pedirle que especifique*/
					mov.tipoMov = pedirCaptura();
					captura = mover (partida, &mov);
				}
				if (captura >= 0) { /* si el movimiento fue válido */
					imprimirMov (&mov);
					imprimirTablero(partida);
					if (!hayCadena(partida)) { /* cambiamos de turno */
						cambiarTurno (partida);
						calcularEstado=1;
					}
				}
				else
					imprimirError(captura);
			}
			else if (jugada == UNDO) {
					quiereUndo = undo(partida);
					if( quiereUndo == OK ){
						imprimirTablero(partida);
						calcularEstado = 1; /*Tiene que volver a chequear el tablero */
					}
					else
						imprimirError(quiereUndo);
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
				if (guardarPartida(partida, nombre) != ERROR)
					printf("Se ha guardado su juego con el nombre '%s'\n", nombre);
				else
					imprimirError(ERR_SAVE);
			}
		}
	}

	eliminarPartida(partida);
	return hayGanador; /* si se salió por QUIT, hayGanador vale 0 */
}

void pedirDimensiones(int * filas, int * columnas){	
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

		printf("Las dimensiones del tablero serán: %d x %d\n\n", cantfils,cantcols);
		printf("¿Desea continuar?\nIngrese S si es así o N para ingresar nuevas dimensiones.\n");

		decision = leerSN();
	} while (decision != 1); /* se desean ingresar nuevas dimensiones */

	*filas=cantfils;
	*columnas=cantcols;
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

enum tJugada pedirJugada(tMovimiento *mov, char *nombre) {
	int n;
	char str[STR_DIM];
	tFlag jugada=OK;

	printf("Ingrese una jugada:\n");

	do {
		printf(" > ");
		n = leerLinea(str, STR_DIM);
		jugada = validarFmtLinea (str, n, mov, nombre);
		imprimirErrorFmt(jugada); /* solo imprime en casos de error */
	} while (jugada < 0); /* hay algún tipo de error en el formato */

	return jugada;
}

tCaptura pedirCaptura (void) {
	char str[5];
	int tipoCaptura;

	printf("Especifique algún tipo de captura, pues el movimiento que quiere realizar admite tanto captura por acercamiento como por alejamiento.\n");

	do {
		printf("Ingrese [w] o [a] > ");
		leerLinea(str, 5);
		tipoCaptura = validarFmtCaptura (str);
	} while (tipoCaptura == ERROR);
	
	return tipoCaptura;
}

tMovimiento pedirCadena (tPartida partida) {
	tFlag esValido = 1;
	tCoordenada origenCadena;
	tMovimiento nuevoMov;
	char str[STR_DIM]; 
	char nuevoStr[15]; /* tamaño suficiente para evaluar si el usuario introdujo de más */
	int fo, co;
	int n;

	origenCadena = consultarOrigenCadena(partida);
	fo = origenCadena.fil+1;
	co = origenCadena.col+1;
	/* sumamos 1 a las coordenadas, pues origenCadena va de 0 fil/col-1, 
	** pero la función de validar formato lee de 1 a fil */ 
	
	printf("\n¡Puede encadenar una movimiento!\n");
	printf("Ingrese solo la coordenada de la casilla a la que desea moverse y el tipo de captura si es necesario.\n");
	printf("Se imprimirá su actual casilla de origen.\n");

	do {
		imprimirErrorFmt(esValido); 
		/* en una primer instancia no imprimirá nada, pues esValido es mayor a 0 */

		printf(" > M [%d,%d]", fo, co);

		n = leerLinea(nuevoStr, 15);

		if (n >= MIN_COORD && n <= MAX_COORD) {
			sprintf(str, "M [%d,%d]%s", fo, co, nuevoStr);
			esValido = validarFmtMov (str, &nuevoMov); /* devuelve menor a 0 en caso de error */
		}
		else
			esValido = ERR_FMT_MOV1;

	} while (esValido < 0); /* si es menor a cero es error */

	return nuevoMov;
}

void pedirNombre (char nombre[]) {
	char str[STR_DIM];
	int n;
	tFlag esValido = 1;

	printf("Escriba el nombre con el que quiere guardar su juego. (Max %d caracteres)\n", MAX_NOM);

	do {
		printf(" > ");
		imprimirErrorFmt(esValido);
		 /* en una primera instancia no imprime nada, pues esValido es mayor o igual a 0*/
		
		n = leerLinea(str, STR_DIM);
		esValido = validarFmtNombre (nombre, str, n);
	} while (esValido < 0);

	return;
}

tFlag leerSN(void) {
	char c;
	char str[3];

	do {
		printf("Ingrese S o N > ");

		leerLinea(str, 3); 
		/* dimensión 3, pues si se ingresa más de S o N str[1] sería distinto de '\0' */
		
		c = toupper(str[0]);
	} while( (c != 'S' && c!= 'N') || str[1] != '\0');
	
	return c == 'S';
}

int validarFmtLinea (char str[], int dim, tMovimiento *mov, char *nombre) {
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

	else if (dim <= MAX_DIM) /* es un posible movimiento */
		jugada = validarFmtMov (str, mov);

	return jugada;
}

int validarFmtMov (const char str[], tMovimiento *mov) {
	const char *p;
	tCaptura captura;

	if (str[0] != 'M' || str[1] != ' ')
		return ERR_FMT;

	p = &str[2]; /* direccion del supuesto primer corchete '[' */

	p = validarFmtCoord(p, &(mov->coordOrig));
	p = validarFmtCoord(p, &(mov->coordDest));

	if (p == NULL)
		return ERR_FMT_MOV1;

	if (*p == '\0') { /* se ingresó un movimiento sin aclaración de captura */
		captura = NINGUNO;
		mov->tipoMov = captura;
	}

	else if ( (captura = validarFmtCaptura(p)) != ERROR ) /* en captura se guarda APPROACH, WITHDRAWAL o ERROR */
		mov->tipoMov = captura;

	if (captura != ERROR)
		return MOV;

	return ERR_FMT_MOV2; /* se introdujo mal el tipo de captura unicamente */
}

const char *validarFmtCoord (const char str[], tCoordenada *coord) {
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
			if (num > MAX_DIM)
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

	coord->fil = filAux-1; /* se resta 1 porque en las funciones del back se cuenta desde 0 a fil-1*/
	coord->col = num-1;

	p = &p[++i]; /* direccion del carácter siguiente al ']' */
	return p;
}

tCaptura validarFmtCaptura (const char str[]) {
	if (str[0] != '[' || ( tolower(str[1]) != 'w' && tolower(str[1]) != 'a' )
	    || str[2] != ']' || str[3] != '\0')
		return ERROR;
	return tolower(str[1]) == 'w' ? WITHDRAWAL : APPROACH;
}

int validarFmtNombre(char destino[], const char origen[], int longOrigen) {
	const char *nuevoNombre;
	int cantEspacios;
	int longNombre;

	nuevoNombre = salteaEspacios(origen, &cantEspacios); 
	/* nuevoNombre apunta el primer caracter distinto de espacio */

	longNombre = longOrigen - cantEspacios; 
	/* longitud del nombre; los espacios iniciales se desestiman */

	if (longNombre >= 1 && longNombre <= MAX_NOM) {
		strcpy(destino, nuevoNombre);
		return SAVE;
	}

	if (longNombre == 0)
		return ERR_FMT_SAVE1; /* se ingresaron solo espacios como nombre */
	return ERR_FMT_SAVE2; /* la longitud del nombre es mayor de lo permitida */
}

const char *salteaEspacios (const char str[], int *cantEspacios) {
	*cantEspacios = 0;
	while (isspace(*str)) {
		str++;
		(*cantEspacios)++;
	}
	return str;
}

void imprimirTablero (const tPartida partida){
	int i, j;
	static char idColor[]={'B', 'N', 'O'}; /*BLANCO, NEGRO, VACIO*/

	for(i=-1; i< numFilas(partida) ; i++)
		{	putchar('\n');
			if(i<0)
				printf("\t\t\t");
			else
				printf("\t\t\t%d\t",i<0?' ':i+1);
			for(j=0; j< numCols(partida) ; j++)
			{	if(i==-1)
				{	if(j==0)
						putchar('\t');
					printf("%-4d",j+1);

					if( j== numCols(partida) -1)
						putchar('\n');
				}
				else
				{	if ( consultarTipo(partida, i, j)==DEBIL)
					   printf("%-4c", tolower( idColor[ consultarOcupante(partida,i,j) ] ));
										 /*BLANCO=0, NEGRO=1, VACIO=2*/
					else 
						printf("%-4c", idColor[ consultarOcupante(partida, i, j) ]);
				}
			}
		}
		putchar('\n');
	return;		
}	

void imprimirMov (const tMovimiento * mov) {
	printf("\n %d,%d -> %d,%d", mov->coordOrig.fil+1, mov->coordOrig.col+1, 
				    mov->coordDest.fil+1, mov->coordDest.col+1);
	return;
}

void imprimirErrorFmt(enum tErrorFmt error){
	
	if (error >= 0) /* no hay error */
		return;

	switch(error) {
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
	default: 
		printf("\aError desconocido\n");
		break;	

	}
}

void imprimirError(int error) {

	if (error >= 0) /* no hay error */
		return;

	switch (error) {
	case ERR_MOV_ORIG:
		printf("\a\n\nError: la casilla elegida no contiene una ficha de su color.\nIntente nuevamente\n");
		break;
	case ERR_MOV_DEST:
		printf("\a\n\nError: la casilla a la que quiere moverse no se encuentra vacía.\nIntente nuevamente\n");
		break;
	case ERR_MOV_TOC:
		printf("\a\n\nError: no puede moverse a una casilla ya visitada en un mismo turno.\nIntente nuevamente\n");
		break;
	case ERR_MOV_DIR:
		printf("\a\n\nError: no puede moverse en la misma dirección del movimiento anterior.\nIntente nuevamente\n");
		break;
	case ERR_MOV_PAIKA:
		printf("\a\n\nError: no puede mover una ficha que no puede realizar una captura cuando existen otras que sí pueden.\nIntente nuevamente\n");
		break;	
	case ERR_MOV_RANGO:
		printf("\a\n\nError: no puede moverse fuera del tablero\nIntente nuevamente\n");
		break;
	case ERR_MOV_NO_ADY:
		printf("\a\n\nError: debe moverse a una casilla adyacente\nIntente nuevamente\n");
		break;
	case ERR_MOV_DEBIL:
		printf("\a\n\nError: desde una casilla debil solo puede moverse arriba, abajo, y a ambos costados\nIntente nuevamente\n");
		break;
	case ERR_MOV_CAD:
		printf("\a\n\nError: debe mover la ficha que se encuentra encadenando en esta jugada\nIntente nuevamente\n");
		break;
	case ERR_UNDO:
		printf("\a\n\nError: no puede realizar UNDO si el juego es entre dos jugadores\n");
		break;
	case ERR_UNDO_DOBLE:
		printf("\a\n\nError: no puede realizar UNDO dos veces seguidas o en el primer turno (¡no hay nada para deshacer!)\n");
		break;
	case ERR_MEM_COMPU:
		printf("\a\n\nError fatal: no hay memoria suficiente para continuar con el juego. Abortando\n");
		break;
	case ERR_SAVE:
		printf("\a\n\nError: no se ha podido guardar su partida\n");
		break;
	default:
		printf("\a\n\nError desconocido\n");
		break;
	}
}
