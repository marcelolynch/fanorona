#include "fanorona.h"
#include <time.h>
#define BLOQUE 20					  

typedef struct {
        char tipo; /* debil o fuerte */
        unsigned char ocupante; /* blanco, negro o vacio*/
        int estado;
} tCasilla;

typedef struct{
        tCasilla ** matriz;
        tCasilla ** matrizAuxiliar;
        int filas;
        int cols;
}tTablero;


struct tJuego{
        tTablero tablero;

        int modo;
        int jugador;
        int hayPaika;
        int hayCadena;
	tCoordenada origenCadena;
        int primerUndo;
        int direccionPrevia;
};



typedef struct {
	tMovimiento * elems;
	size_t dim;
 } tVecMovs;	


enum tEstado {LIBRE=0, TOCADA};
enum tDireccion {N=1, S, E, O, NE, NO, SE, SO, NULA};


static void rellenarTablero(tTablero * tablero);
static void liberarTodo(tCasilla ** matriz, int n);
static void limpiarTocadas(tTablero * tablero);
static void actualizarTablero(tPartida partida, tMovimiento * mov);
static void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion);
static int aumentarPosibles(const tPartida partida, tVecMovs * movsPosibles, tCoordenada casillaOrig);
static enum tDireccion direccionDestino(tCoordenada origen, tCoordenada destino);
static int meCapturan(const tTablero *tablero, tCoordenada posicion, int jugador);
static int hayComida (int jugador, const tTablero *tablero, tCoordenada origen, enum tDireccion direccion);
static int validarMovimiento(tPartida partida, tMovimiento * movimiento);
static int jugadaObligada(const tPartida partida, int jugador, tCoordenada origen);
static void copiarTablero(tTablero * tablero);
static void intercambiarTableros(tTablero * tablero);
static tCasilla ** generarMatrizTablero(int fils, int cols);
static int fueraDeRango(const tTablero * tablero, int f, int c);
static tTablero generarTablero(int fils, int cols, int modo);


static int haySeed=0;

/* Funciones que devuelven informacion
** sobre la partida actual (campos de la estructura)*/

tFlag hayCadena(tPartida partida){
	return partida->hayCadena;
}

int jugadorActual(tPartida partida){
	return partida->jugador;
}

int modoJuego(tPartida partida){
	return partida->modo;
}

int consultarOcupante(tPartida partida, int f, int c){
	return partida->tablero.matriz[f][c].ocupante; 
}

int consultarTipo(tPartida partida, int f, int c){
	return partida->tablero.matriz[f][c].tipo; 
}

tCoordenada consultarOrigenCadena (tPartida partida) {
	return partida->origenCadena;
}

int numFilas(tPartida partida){
	return partida->tablero.filas;
}

int numCols(tPartida partida){
	return partida->tablero.cols;
}


static int fueraDeRango(const tTablero * tablero, int f, int c){
	/* Booleano: verdadero si la coordenada (f,c) esta 
	** dentro del tablero, */
	return  (f < 0 || c < 0 || f >= tablero->filas || c  >= tablero->cols);
}

int mover (tPartida partida, tMovimiento * movimiento) {
	/* Mover recibe una estructura de movimiento y la valida.
	** Si la validacion devuelve un error, mover lo devuelve en su nombre.
	** Si hay ambiguedad en el movimiento, se devuelve AMBIGUO y no se hace nada.
	** Si no se encuentran errores, se ejecuta la movida y se actualiza el tablero
	** controlando al mismo tiempo si se entro en una cadena (se actualiza el campo
	** hayCadena. Luego de ejecutar la movida, se devuelve el tipo de captura realizada
	** (WITHDRAWAL, APPROACH, NINGUNO).
	*/
	int captura;
	int jugador = partida->jugador;

	if(partida == NULL)
		return ERROR;

	captura = validarMovimiento(partida, movimiento);

	if (captura == AMBIGUO) { 
		/* si la jugada es ambigua, retorna hasta que se pase el movimiento completo */
		return captura;
	}
	else if (captura == WITHDRAWAL || captura == APPROACH || captura == NINGUNO) { /* si no fue error */
		/* Copia el tablero al auxiliar antes en el comienzo 
		** del turno  del usuario y si juega vs Computadora*/
		if (!(partida->hayCadena) && partida->modo == PVE && partida->jugador == BLANCO)
			copiarTablero(&partida->tablero);
		
		partida->tablero.matriz[movimiento->coordOrig.fil][movimiento->coordOrig.col].estado=TOCADA;
		partida->direccionPrevia = direccionDestino(movimiento->coordOrig, movimiento->coordDest); 

		movimiento->tipoMov = captura; /* captura vale WITHDRAWAL o APPROACH */

		actualizarTablero(partida, movimiento);

		partida->hayCadena = 0; /* En principio se asume que no hay */

		if (captura != NINGUNO) /* si no fue una movida PAIKA, busca una posible cadena */
			partida->hayCadena = jugadaObligada(partida, jugador, movimiento->coordDest);

		if (partida->hayCadena) {
			partida->origenCadena = movimiento->coordDest;
				/* Coordenada origen del proximo movimiento en la cadena */
		}
	}

	return captura;
}

int estadoJuego(const tPartida partida){
	/* Controla el tablero para ver si algun jugador quedo sin fichas,
	** si existen movimientos obligados (si no los hay, se habilita el paika)
	** y si el jugador que esta por jugar tiene todas movidas bloqueadas (mover
	** sin comer, quedando en una posicion vulnerable), en cuyo caso el juego debe 
	** terminar en empate.
	** Devuelve SEGUIR, GANADOR_BLANCO, GANADOR_NEGRO o EMPATE segun corresponda */
	
	int jugador;
	int hayBlancos=0, hayNegros=0;
	int ocupante;
	int i,j,n;
	int hayMovimientos=0;
	tCoordenada origen;
	tCoordenada ady;
	enum tDireccion direcciones[]={N, S, E, O, NE, NO, SE, SO};
	int dir, dirsPosibles, dirFil, dirCol;
	int estado;

	if(partida == NULL)
		return ERR_PARAMS; /* Antes que segfault... */		

	jugador = partida->jugador;
	partida->hayPaika = 1; /* Asumo que hay */

	for(i=0; i< partida->tablero.filas ; i++)
	for(j=0; j< partida->tablero.cols ; j++){

		if((ocupante = partida->tablero.matriz[i][j].ocupante) != VACIO) {
			if(ocupante==BLANCO){
				hayBlancos=1;
			}
			else{
				hayNegros=1;
			}


			/* Voy a analiar si esta casilla esta en una posicion desfavorable 
			** (bloqueada) en la proxima movida: es decir, si no puede capturar 
			** nada en la siguiente movida que haga, y que no importa donde se mueva
			** alguien lo captura. Si todas las piezas del jugador que va a jugar cumplen 
			** con esto, el juego termina en empate. */
			if(ocupante==jugador){
				origen.fil = i;
				origen.col = j;
				if(jugadaObligada(partida, ocupante, origen)){
					partida->hayPaika=0;
					hayMovimientos=1;
				}
				/* Si puede comer en la jugada posterior, la casilla no esta bloqueada
				** (esta obligada a comer aunque sea mala estrategia); si la jugada no 
				** es obligada, analizo para cada direccion si la captura es inminente 
				** en caso de dirigirse a ella */
				else{
					dirsPosibles = partida->tablero.matriz[i][j].tipo == FUERTE ? 8:4;
					for(n=0; n<dirsPosibles ; n++){
						dir = direcciones[n];
						incrementoSegunDir(&dirFil, &dirCol, dir);
															
				
					if( !fueraDeRango(&partida->tablero, i+dirFil, j+dirCol) ){
						ady.fil=i+dirFil;
						ady.col=j+dirCol;
						if(partida->tablero.matriz[ady.fil][ady.col].ocupante==VACIO 
							&& !meCapturan(&partida->tablero, ady, ocupante)) 
							/* Existe un movimiento en el que no se ve amenazado 
							** en la jugada posterior */
							hayMovimientos=1; 
					}
					}
				}	
	  		}
		}			
	}

	if(!hayBlancos)
		estado = GANADOR_NEGRO;
	else if(!hayNegros)
		estado = GANADOR_BLANCO;
	else if(!hayMovimientos)
		estado = EMPATE;
	else
		estado = SEGUIR;


	return estado;
}

void cambiarTurno (tPartida  partida) {
	/* Realiza las tareas de limpieza correspondientes
	** al cambio de turno: se cambia el jugador actual
	** se limpian las casillas tocadas en la cadena
	** se reseta la direccion a NULA y se habilita undo */

	partida->jugador = !(partida->jugador);
	limpiarTocadas(&partida->tablero);
	partida->direccionPrevia = NULA; /* Ninguna*/
	partida->primerUndo=1;
}


int undo(tPartida  partida){
	/* Si el undo es posible, lo realiza intercambiando los tableros
	** si no es posible, devuelve el error correspondiente */
	int estado;

	if(partida->modo != PVE)
		/* No puede hacer undo si estan jugando dos jugadores*/
		return ERR_UNDO; 

	if(partida->primerUndo){
		intercambiarTableros(&partida->tablero);
		partida->primerUndo=0;
		estado = OK;
	}

	else
		estado = ERR_UNDO_DOBLE; /* Es el segundo undo consecutivo */

	return estado;
}

int calcularMovCompu(tMovimiento * mov, const tPartida  partida){

	/* Genera un vector dinamico de tipo tVecMovs con todos 
	** los movimientos que puede realizar la computadora en este turno
	** elige uno al azar, y lo devuelve en el parametro de salida mov.
	*/	

	tTablero * tablero = &partida->tablero;
	tVecMovs movsPosibles;
	int i,j;
	int eleccion;
	tCoordenada casillaOrig;
        
	movsPosibles.dim = 0;
	movsPosibles.elems = NULL;

	if(!partida->hayCadena){

		for(i=0 ; i<tablero->filas ; i++)
			for(j=0 ; j<tablero->cols ; j++)
				if(tablero->matriz[i][j].ocupante == NEGRO){
					casillaOrig.fil=i;
					casillaOrig.col=j;
					if( aumentarPosibles(partida, &movsPosibles, casillaOrig) != 0)
						return 1;		
					}
	}
	else{
		/* Si estoy encadenando, la coordenada de origen esta ya 
		** determinada en partida->origenCadena. 
		** Asi, se usa como origen. */
		aumentarPosibles(partida, &movsPosibles, partida->origenCadena);
	}

	eleccion = rand()%movsPosibles.dim;
	*mov = movsPosibles.elems[eleccion];
	return 0;
}

static int aumentarPosibles(const tPartida partida, tVecMovs * movsPosibles, tCoordenada casillaOrig){
       	/* Rellena el vector movsPosibles con los posibles movimientos que puede
	** realizar la computadora desde la casillaOrig  */
 
	tTablero * tablero = &partida->tablero;
	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	int dirsPosibles, dirFil, dirCol;	
	int n;
	tMovimiento * aux;
	int tipoComida;
	enum tDireccion dir;
	
	dirsPosibles = tablero->matriz[casillaOrig.fil][casillaOrig.col].tipo==FUERTE ? 8 : 4;
	
	for(n=0; n<dirsPosibles ; n++){
		dir = direcciones[n];

		if(dir != partida->direccionPrevia){
			incrementoSegunDir(&dirFil, &dirCol, dir);
						
			if(!fueraDeRango(tablero, casillaOrig.fil + dirFil, casillaOrig.col + dirCol) 
				&& tablero->matriz[casillaOrig.fil + dirFil][casillaOrig.col + dirCol].estado != TOCADA ){

				if( (tipoComida = hayComida(NEGRO, tablero, casillaOrig, dir)) != NINGUNO
					 || partida->hayPaika ){

					if(movsPosibles->dim % BLOQUE == 0){
						aux=realloc(movsPosibles->elems, (movsPosibles->dim + BLOQUE)*sizeof(*aux));
				
						if(aux==NULL)
							return 1;
					
						movsPosibles->elems = aux;
						}
		
					movsPosibles->elems[movsPosibles->dim].coordOrig.fil = casillaOrig.fil;
					movsPosibles->elems[movsPosibles->dim].coordOrig.col = casillaOrig.col;
					movsPosibles->elems[movsPosibles->dim].coordDest.fil = casillaOrig.fil+dirFil;
					movsPosibles->elems[movsPosibles->dim].coordDest.col = casillaOrig.col+dirCol;

					if(tipoComida == AMBIGUO) /*Elijo una al azar*/
						tipoComida = rand()%2?APPROACH:WITHDRAWAL;

					movsPosibles->elems[movsPosibles->dim].tipoMov = tipoComida;
		
					movsPosibles->dim++;
				}
			}
	}
    }

	return 0;
}

int guardarPartida(tPartida partida, const char * nombre){

	/* Guarda la partida en el directorio actual con el nombre
	** que se recibe en el string nombre */

	FILE *f;
	int nfilas, ncols, jugador, modo;
	int i, j;

	if(partida == NULL)
		return ERR_PARAMS;

	nfilas=partida->tablero.filas;
	ncols=partida->tablero.cols;
	
	jugador = partida->jugador + 1; 
	modo = partida->modo;
	/* Debe ser 1 para el jugador 1 (que esta alojado como 0), y 2 para el jugador 2 */

	if((f=fopen(nombre, "wb")) == NULL)
		return ERR_SAVE;

	fwrite(&modo, sizeof(int),1,f);
	fwrite(&jugador,sizeof(int),1,f);
	fwrite(&nfilas, sizeof(int),1,f);
	fwrite(&ncols, sizeof(int),1,f);
	for(i=0; i<nfilas ; i++)
		for(j=0; j<ncols ; j++){
			switch(partida->tablero.matriz[i][j].ocupante){
				case BLANCO:
				fputc('&', f); break;
				case NEGRO:
				fputc('#', f); break;
				case VACIO:
				fputc('0', f); break;
			}
		}	
	fclose(f);
	return 1;
}

tPartida cargarPartida(const char * nombre){
	/* Abre el archivo con el nombre pasado (del directorio local),
	** y genera una estructura de juego a partir del archivo. Devuelve en su nombre
	** el puntero a dicha estructura. 
	** Se asume que los datos estan validados y el archivo no esta corrupto, aunque
	** si se controla la aparicion de caracteres incorrectos a la hora del llenado del
	** tablero. En caso de cualquier error, devuelve NULL*/

	int i,j,c;
	int fils, cols;
	FILE * f;
	tPartida partida=malloc(sizeof(*partida));
	
	if(partida == NULL)
		return NULL;
	
	
	if((f=fopen(nombre,"rb"))==NULL)
		{free(partida);
		return NULL;
		}
	c=0;

	c+= fread(&partida->modo, sizeof(int),1, f);
	c+= fread(&partida->jugador, sizeof(int),1,f);
	c+= fread(&fils, sizeof(int), 1, f);
	c+= fread(&cols, sizeof(int), 1, f);
	
	if(c<4)
		return NULL;

	partida->tablero.filas = fils;
	partida->tablero.cols = cols;

	(partida->jugador)--; 
	/* En el savefile estan guardados como 1,2. En la logica se manejan como 0,1*/
	

        partida->tablero.matriz=generarMatrizTablero(fils, cols);

	if(partida->modo == PVE)
		partida->tablero.matrizAuxiliar = generarMatrizTablero(fils, cols);

	if(partida->tablero.matriz==NULL 
		|| (partida->modo == PVE && partida->tablero.matrizAuxiliar==NULL))
		return NULL;


	for(i=0, c=0; i<partida->tablero.filas ; i++)
		for(j=0 ; j<partida->tablero.cols ; j++){
			if (i%2 == j%2)
                        	partida->tablero.matriz[i][j].tipo= FUERTE;
                	else
                        	partida->tablero.matriz[i][j].tipo= DEBIL;

			switch(fgetc(f)){
				case '0':
				partida->tablero.matriz[i][j].ocupante=VACIO;
				break;	
				case '&':
				partida->tablero.matriz[i][j].ocupante=BLANCO;
				break;	
				case '#':
				partida->tablero.matriz[i][j].ocupante=NEGRO;
				break;	
				default:
				liberarTodo(partida->tablero.matriz, partida->tablero.filas);
				if(partida->modo == PVE)
					liberarTodo(partida->tablero.matrizAuxiliar, partida->tablero.filas);

				return NULL;
				break;
 			}
		}
	fclose(f);
	return partida;
}


tPartida generarPartida(int fils, int cols, int modo){
	/* Reserva memoria para la estructura y devuelve
	** el puntero a la misma en su nombre */

	tPartida partida = malloc(sizeof(*partida));	

	/* Esta validacion deberia estar hecha por el front-end, pero 
	** nunca se sabe */
	if( fils < MIN_DIM || fils > MAX_DIM || cols < MIN_DIM || cols > MAX_DIM
		 || cols%2==0 || fils%2 == 0 || cols<fils || (modo != PVE && modo != PVP))
		return NULL;

	if(partida != NULL){
		partida->modo = modo;
		partida->direccionPrevia=NULA;
		partida->hayCadena=0;
		partida->hayPaika=0;
		partida->primerUndo=0;

		partida->tablero = generarTablero(fils, cols, modo);
	}
	
	if(!haySeed){
		/*La semilla se planta solo con la inicializacion
		** de la primera partida */
		srand(time(NULL));
		haySeed=1;
	}
	return partida;		
}


void eliminarPartida(tPartida partida){
	/* Libera la memoria de la partida*/

	liberarTodo(partida->tablero.matriz, partida->tablero.filas);

	if(partida->modo == PVE) /*Modo vs Computadora: libero la auxiliar */
		liberarTodo(partida->tablero.matrizAuxiliar, partida->tablero.filas);

	free(partida);
	return;
}

static tTablero generarTablero(int fils, int cols, int modo){
	/* Inicializa una estructura tablero y la devuelve en su nombre
	** Si modo es PVE, ademas del tablero principal genera el auxiliar */	

	tTablero tablero;
	
	tablero.matriz=generarMatrizTablero(fils, cols);
	if(tablero.matriz == NULL)
		return tablero;

	tablero.filas=fils;
	tablero.cols=cols;
	
	rellenarTablero(&tablero);

	if(modo==PVE){
		tablero.matrizAuxiliar = generarMatrizTablero(tablero.filas, tablero.cols);
		if(tablero.matrizAuxiliar == NULL){
			liberarTodo(tablero.matriz, tablero.filas);	
			tablero.matriz = NULL; 
				/*Tablero principal en NULL marca el error*/
		}
	}
	return tablero;

}

static tCasilla ** generarMatrizTablero(int fils, int cols){
	/* Reserva la memoria para una matriz de casillas
	** (tablero de fils x cols) y la devuelve en su nombre */

	tCasilla ** matriz;
	int i;
	
	matriz=malloc(fils *sizeof(*matriz));

	if (matriz == NULL)
		return NULL;
		
	for(i=0; i < fils; i++)
	{	
		matriz[i]= malloc(cols*sizeof(tCasilla));
				
		if(matriz[i] ==NULL){
			liberarTodo(matriz, i);
			return NULL;
		}	
	}

	return matriz;
}

static void copiarTablero(tTablero * tablero){
	/* Copia el contenido del tablero principal
	** al auxiliar 	*/
	int i, j;
	for(i=0; i<tablero->filas ; i++)
		for(j=0; j<tablero->cols ; j++)
			tablero->matrizAuxiliar[i][j] = tablero->matriz[i][j];

}	

static void intercambiarTableros(tTablero * tablero){
	/*Intercambia el tablero en uso por el auxiliar,
	** mediante el intercambio de a que zona de memoria apunta 
	** el puntero al tablero principal con el auxiliar*/
	tCasilla ** aux;
	aux = tablero->matrizAuxiliar;
	tablero->matrizAuxiliar = tablero->matriz;
	tablero->matriz = aux;

}

static void rellenarTablero(tTablero * tablero){
	/* Rellena el tablero con la disposicion inicial */

	int i,j;
	int postCentral=0;
	for(i=0; i<tablero->filas ; i++){
	for(j=0; j<tablero->cols; j++)
		{	
		if (i%2 == j%2)
			tablero->matriz[i][j].tipo= FUERTE;
		else
			 tablero->matriz[i][j].tipo= DEBIL;

		
		if (i < tablero->filas/2)
			tablero->matriz[i][j].ocupante = NEGRO;
		else if (i > tablero->filas/2)
			tablero->matriz[i][j].ocupante= BLANCO;
	
		else if (i == tablero->filas/2 && j != tablero->cols/2) /*Fila central (menos casilla central) */
		{	
			if (j%2==0)
				/*No hay simetria con respecto a la central; se invierte
				**  la paridad del blanco/negro luego de la misma (postCentral) */
				tablero->matriz[i][j].ocupante = postCentral ? BLANCO:NEGRO; 
			else
				tablero->matriz[i][j].ocupante = postCentral ? NEGRO:BLANCO;
			
		}
		else{	/*Casilla central*/
			tablero->matriz[i][j].ocupante= VACIO;
			postCentral=1; /*Relevante para colocar las otras piezas*/
		}	
	}	
	}
	
	return;
}

static void liberarTodo(tCasilla ** matriz, int n){
	/* Libera la memoria reservada para una matriz 
	** de casillas (tablero) */	

	int i;
	for(i=0; i<n ; i++)
		free(matriz[i]);
	
	free(matriz);
			  
}

static void limpiarTocadas(tTablero * tablero){
	/* Las casillas tocadas en una cadena
	** deben ser puestas en LIBRE antes de hacer
	** el chequeo general sobre el tablero, ya que la
	** condicion de TOCADA afecta al algoritmo de jugadaObligada,
	** que se utiliza para controlar si hay jugadas obligadas 
	** en el siguiente turno, o si se entra al estado de Paika */

	int i,j;
	for(i=0; i<tablero->filas ; i++)
		for(j=0; j<tablero->cols ; j++)
			if(tablero->matriz[i][j].estado == TOCADA)
				tablero->matriz[i][j].estado = LIBRE;
}

static void actualizarTablero(tPartida partida, tMovimiento * mov){
	/* Mueve la ficha y recorre el tablero en la direccion del movimiento
	** y cambia las casillas del enemigo por VACIO hasta encontrar
	** una que tenga un ocupante distinto del enemigo, o se vaya del 
	** rango del tablero */

	tTablero * tablero = &partida->tablero; 
	int i, j;
	int dirFil, dirCol;
	int fini, cini;
	int jugador = tablero->matriz[mov->coordOrig.fil][mov->coordOrig.col].ocupante;
	int enemigo = !jugador;
	/* La direccion del movimiento a realizar se guardó en direccionPrevia 
	** cuando se llamó a validarMovimiento */
	enum tDireccion direccion = partida->direccionPrevia;

	incrementoSegunDir(&dirFil, &dirCol, direccion);
	
	if(mov->tipoMov==WITHDRAWAL){
		fini=mov->coordOrig.fil - dirFil; /*De donde empieza a comer*/
		cini=mov->coordOrig.col - dirCol;
		dirFil = -dirFil;	/*Se invierte la direccion a recorrer*/
		dirCol = -dirCol;
	}

	else if(mov->tipoMov==APPROACH){
		fini=mov->coordDest.fil + dirFil; /*De donde empieza a comer*/
		cini=mov->coordDest.col + dirCol;
		}
	tablero->matriz[mov->coordDest.fil][mov->coordDest.col].ocupante = jugador;
	tablero->matriz[mov->coordOrig.fil][mov->coordOrig.col].ocupante = VACIO;  /*Movi la ficha*/

	i=fini;
	j=cini;
	while((i<(tablero->filas) && i>=0 && j<(tablero->cols) && j>=0) && tablero->matriz[i][j].ocupante==enemigo){
		/*Mientras no me vaya del tablero y siga habiendo enemigos en la linea de captura*/
		tablero->matriz[i][j].ocupante = VACIO;
		
		i+=dirFil;
		j+=dirCol;
	}
}

static void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion){
	/* Devuelve el incremento en fila y columna para una iteracion
	** que quiera recorrer el tablero en una direccion determinada*/
 switch(direccion){
                case N: *dirFil=-1;
                        *dirCol=0;
                break;
                case S: *dirFil=1;
                        *dirCol=0;
                break;
                case E: *dirFil=0;
                        *dirCol=1;
                break;
                case O: *dirFil=0;
                        *dirCol=-1;
                break;
                case NE: *dirFil=-1;
                         *dirCol=1;
                break;
                case NO: *dirFil=-1;
                         *dirCol=-1;
                break;
                case SE: *dirFil=1;
                         *dirCol=1;
                break;
                case SO: *dirFil=1;
                         *dirCol=-1;
                break;
		default:
		break;
        }
}


static enum tDireccion direccionDestino(tCoordenada origen, tCoordenada destino){

	/* Devuelve la direccion que lleva un movimiento
	** con las coordenadas de origen y destino que se pasan 
	** como parametro  */

	enum tDireccion direccion;
	char fo=origen.fil;
	char co=origen.col;
	char fd=destino.fil;
	char cd=destino.col;
	
	if(fd==fo-1 && cd==co)
		direccion=N;
	else if(fd==fo+1 && cd==co)
		direccion=S;
	else if(fd==fo && cd==co-1)
		direccion=O;
	else if(fd==fo && cd==co+1)
		direccion=E;
	else if(fd==fo-1 && cd==co-1)
		direccion=NO;
	else if(fd==fo-1 && cd==co+1)
		direccion=NE;
	else if(fd==fo+1 && cd==co-1)
		direccion=SO;
	else if(fd==fo+1 && cd==co+1)
		direccion=SE;
	else
		direccion=ERROR; /* Esta queriendo ir a casilla no adyacente */
	return direccion;


}

static int meCapturan(const tTablero *tablero, tCoordenada posicion, int jugador){
	/* Devuelve un booleano indicando si una ficha del color <<jugador>> 
	** esta amenazada desde la coordenada <<posicion>>, es decir, que es
	** capturada en el turno del oponente estando alli */

	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	/* dirsPosibles marca hasta donde se recorrera el vector de direcciones i
	** para el chequeo. Si la casilla es debil, no se chequean las diagonales*/
	int dirsPosibles = tablero->matriz[posicion.fil][posicion.col].tipo == FUERTE ? 8:4;
	int i, dirFil, dirCol;
	enum tDireccion dir;
	tCoordenada adyacente, siguienteAdy;	
	int enemigo = !jugador;

	for(i=0; i<dirsPosibles ; i++){
		dir = direcciones[i];
		incrementoSegunDir(&dirFil, &dirCol, dir);
		
		/*Las casillas que pueden comerme por approach o widthrawal en el proximo turno,
		** en la direccion dir */
		adyacente.fil = posicion.fil + dirFil; /* Amenaza por widthdrawal */
		adyacente.col= posicion.col + dirCol; 

		siguienteAdy.fil = adyacente.fil + dirFil; /* Amenaza por approach */
		siguienteAdy.col = adyacente.col + dirCol;

		 /* Se chequean adyacente y siguienteAdy para comprobar  que esten dentro del tablero.
		 ** La captura por withdrawal moveria de adyacente a siguienteAdy, por lo
		 ** tanto para el widthdrawal ambos deben estar en el tablero.
		 ** Para el approach, siguienteAdy debe ser una casilla valida, y por lo tanto
		 ** adyacente tambien.
		 */
		if(!fueraDeRango(tablero, adyacente.fil, adyacente.col) 
			&& !fueraDeRango(tablero, siguienteAdy.fil, siguienteAdy.col)){

			if(tablero->matriz[adyacente.fil][adyacente.col].ocupante == enemigo
				&& tablero->matriz[siguienteAdy.fil][siguienteAdy.col].ocupante == VACIO){
					/*Me comen por WITHDRAWAL en la proxima jugada en esa direcicon*/
					return 1;
			}	
			if(tablero->matriz[adyacente.fil][adyacente.col].ocupante == VACIO
				&& tablero->matriz[siguienteAdy.fil][siguienteAdy.col].ocupante == enemigo){
					/*Me comen por APPROACH en la proxima jugada, desde esa direccion*/
					return 1;
		}
		}
	}

	return 0; /*Si llegue hasta aca, no hay manera de que me capturen*/	

}

static int hayComida (int jugador, const tTablero *tablero, tCoordenada origen, enum tDireccion direccion) { 

	/* Devuelve el tipo de captura posible segun el movimiento que quiera realizar el usuario
	** (direccion del movimiento desde de casilla origen). Si no encuentra, devuelve NINGUNO. 
	** Si la casilla a la que se quiere mover esta ocupada, devuelve NINGUNO (no devuelve error);
	** en definitiva no puede comer moviendose alli, y eso ya viene validado desde validarMovimiento
	*/	

	int fdA, fdW, cdA, cdW;
	int fueraDeRangoA, fueraDeRangoW;
	char fo = origen.fil;
        char co=origen.col;
        char enemigo = !jugador;
	int captura;

        int dirFil, dirCol;
        tFlag hayAppr=0, hayWithdr=0;
	
	incrementoSegunDir(&dirFil, &dirCol, direccion);
	
	fdA=fo+2*dirFil; /*Fila de la casilla a capturar por approach (target) */
	cdA=co+2*dirCol; /*Columna*/
	fdW=fo-dirFil; /*Fila de la casilla a capturar por withdrawal (target)*/
	cdW=co-dirCol; /*Columna*/

	/* Reviso si los casilleros que tienen las fichas a capturar existen, 
	** y si la casilla a moverse esta vacia.
	** Si no, el movimiento es invalido (NINGUNO) */ 
		
	
	fueraDeRangoA = ( fueraDeRango(tablero, fdA,cdA) || fueraDeRango(tablero, fo+dirFil, co+dirCol)
			|| tablero->matriz[fo+dirFil][co+dirCol].ocupante != VACIO);
	
	fueraDeRangoW = ((fueraDeRango(tablero, fdW,cdW) || fueraDeRango(tablero, fo+dirFil, co+dirCol))
			 || tablero->matriz[fo+dirFil][co+dirCol].ocupante != VACIO);

	if (!fueraDeRangoA && tablero->matriz[fdA][cdA].ocupante== enemigo)
                /*El elemento de la matriz corresponde a la casilla a capturar por approach*/
	       {hayAppr = 1;
		}
        if (!fueraDeRangoW && tablero->matriz[fdW][cdW].ocupante == enemigo)
                /*El elemento de la matriz corresponde a la casilla a capturar por withdrawal*/
                {hayWithdr = 1;
		 
		}

        if (hayAppr && hayWithdr)
                captura=AMBIGUO; /* Podrian ser las dos */
        else if (hayAppr)
                captura=APPROACH;
        else if (hayWithdr)
                captura=WITHDRAWAL;
        else
                captura=NINGUNO;

	return captura;
}

static int validarMovimiento(tPartida partida, tMovimiento * movimiento) {
	/* Valida todos los errores posibles de la jugada que le pasan
	** En caso de error, lo devuelve. En caso de que no haya error,
	** devuelve en movimiento->tipoMov el tipo de captura posibile 
	** (resuelve la ambiguedad en el caso que pueda). Si no puede
	** resolver la ambiguedad, devuelve AMBIGUO */
		
	int jugador = partida->jugador;
	tTablero * tablero = &partida->tablero;
	int jugada, aux;

	int fo=movimiento->coordOrig.fil;
	int co=movimiento->coordOrig.col;
	int fd=movimiento->coordDest.fil;
	int cd=movimiento->coordDest.col;
	
	enum tDireccion direccionMov;

	int mov = movimiento->tipoMov;
	if ( ! (mov == APPROACH || mov == WITHDRAWAL || mov == NINGUNO || mov == AMBIGUO) )
		/* No es valido el dato */
		return ERR_PARAMS;
	
	if(fd < 0 || fo < 0 || co < 0 || cd < 0 || fo >= tablero->filas || fd >= tablero->filas 
	   || co >= tablero->cols || cd >= tablero->cols)
		return ERR_MOV_RANGO; /*Fuera de limites del tablero*/

	if(jugador != tablero->matriz[fo][co].ocupante)
		return ERR_MOV_ORIG; /* No puede mover la ficha porque no es suya */

	if(tablero->matriz[fd][cd].ocupante != VACIO)
		return ERR_MOV_DEST; /* No puede mover porque la casilla no esta vacia */

	if(tablero->matriz[fd][cd].estado==TOCADA)
		return ERR_MOV_TOC;

	if (partida->hayCadena && (fo != partida->origenCadena.fil || co != partida->origenCadena.col) )
		/* Estamos en medio de un encadenamiento y pide mover una ficha que no debe */
		return ERR_MOV_CAD;

	direccionMov = direccionDestino(movimiento->coordOrig, movimiento->coordDest);

	if(direccionMov == ERROR)
		return ERR_MOV_NO_ADY; /*No es una direccion valida*/

	if(tablero->matriz[fd][cd].tipo == DEBIL && (direccionMov > O)) /* NE, NO, SE, SO */  
		return ERR_MOV_DEBIL;

	if(direccionMov == partida->direccionPrevia)
		return ERR_MOV_DIR; /*No puede moverse en la misma direccion en la que venia moviendose */	

	if( (aux=hayComida(jugador, tablero, movimiento->coordOrig, direccionMov)) != NINGUNO
	    || partida->hayPaika )
		/*Solamente chequeo la situacion de Paika si no selecciono un movimiento donde pueda comer */
		jugada=aux;
	else
		return ERR_MOV_PAIKA;

	
	if(jugada==AMBIGUO && movimiento->tipoMov!=NINGUNO)
		/*Si el jugador puede hacer ambas cosas pero ya eligio*/
		jugada=movimiento->tipoMov;
	
	return jugada; /* Devuelve WITHDRAWAL, APPROACH o AMBIGUO si no se entra al ultimo if*/
}


static int jugadaObligada(const tPartida partida, int jugador, tCoordenada origen){
	/* Devuelve un booleano, verdadero si desde la coordenada origen 
	** el jugador puede (y por lo tanto debe) realizar una captura */

	tTablero * tablero = &partida->tablero;
	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	int dir;
	int dirFil, dirCol;
	tCoordenada destino;
	int dirsPosibles = tablero->matriz[origen.fil][origen.col].tipo == FUERTE ? 8:4; 

	for(dir=0 ; dir<dirsPosibles ; dir++){
		incrementoSegunDir(&dirFil, &dirCol, direcciones[dir]);
		destino.fil = origen.fil + dirFil;
		destino.col = origen.col + dirCol;
		
		if(destino.fil<(tablero->filas) && destino.fil>=0 && destino.col<(tablero->cols) && destino.col>=0){

		if(tablero->matriz[destino.fil][destino.col].estado != TOCADA 
		   && direcciones[dir] != partida->direccionPrevia)
		 /* Solo chequeo si hay comida si no visite esa casilla antes 
		 ** y no es la misma dirección de antes */
			if(hayComida(jugador, tablero, origen, direcciones[dir])!=NINGUNO)
				return 1; /*Debe capturar esa pieza la proxima jugada */
		}
	}

	return 0;
}
