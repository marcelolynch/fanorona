#include "fanorona.h"
#define BLOQUE 20					  

typedef struct {
	tMovimiento * elems;
	size_t dim;
 } tVecMovs;	

static void rellenarTablero(tTablero * tablero);
static void liberarTodo(tCasilla ** matriz, int n);
static void limpiarTocadas(tTablero * tablero);
static void actualizarTablero(tTablero * tablero, enum tDireccion direccion, tMovimiento mov);
static void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion);
static int aumentarPosibles(tTablero * tablero, tVecMovs * movsPosibles, tCoordenada casillaOrig, tFlag hayPaika, enum tDireccion direccionPrevia);
static enum tDireccion direccionDestino(tCoordenada origen, tCoordenada destino);
static int meCapturan(tTablero *tablero, tCoordenada posicion, int jugador);
static enum tCaptura hayComida (int jugador, tTablero *tablero, tCoordenada origen, enum tDireccion direccion);
static int validarMovimiento(int jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia, tFlag hayPaika);
static int jugadaObligada(tTablero * tablero, int jugador, tCoordenada origen, enum tDireccion dirAnterior);



int mover (char jugador, int modo, tTablero * tablero, tCasilla ** tableroAuxiliar, tMovimiento * movimiento, enum tDireccion * direccionPrevia, tFlag hayPaika, tFlag * hayCadena) {
	int captura;
	
	captura = validarMovimiento(jugador, tablero, *movimiento, direccionPrevia, hayPaika);

	if (captura == AMBOS) { /* si la jugada es ambigua, retorna para que el front pida la captura */
		*direccionPrevia = NULA;
		return captura;
	}
	else if (captura >= 0) { /* si no fue error */
		/*Copia el tablero al auxiliar antes en el comienzo del turno  del usuario y si juega vs Computadora*/
		if (!(*hayCadena) && modo == PVE && jugador == BLANCO)
			copiarTablero(tablero, tableroAuxiliar);
		
		movimiento->tipoMov = captura;
		actualizarTablero(tablero, *direccionPrevia, *movimiento);
		*hayCadena = 0;

		if (captura != NINGUNO) /* si no fue PAIKA, busca una posible cadena */
			*hayCadena = jugadaObligada(tablero, jugador, movimiento->coordDest, *direccionPrevia);

		if (*hayCadena) {
			/* al haber cadena, el orígen es el nuevo destino */
			movimiento->coordOrig.fil = movimiento->coordDest.fil;
			movimiento->coordOrig.col = movimiento->coordDest.col;
			tablero->matriz[movimiento->coordOrig.fil][movimiento->coordOrig.col].estado = ACTIVA; /*La casilla esta en el medio de una cadena*/
		}
	}

	return captura;
}

int estadoJuego(tTablero * tablero, int jugador, tFlag * hayPaika){
	int hayBlancos=0, hayNegros=0;
	int ocupante;
	int i,j,n;
	int hayMovimientos=0;
	tCoordenada origen;
	tCoordenada ady;
	enum tDireccion direcciones[]={N, S, E, O, NE, NO, SE, SO};
	int dir, dirsPosibles, dirFil, dirCol;
	int estado;

	*hayPaika = 1;

	for(i=0; i<tablero->filas ; i++)
	for(j=0; j<tablero->cols ; j++){

		if((ocupante = tablero->matriz[i][j].ocupante) != VACIO) {
			if(ocupante==BLANCO){
				hayBlancos=1;
			}
			else{
				hayNegros=1;
			}


			/*Voy a analiar si esta casilla esta en una posicion desfavorable (bloqueada) en la proxima movida
			** es decir, si no puede capturar nada en la siguiente movida que haga, y que no importa donde se mueva
			** alguien lo captura. Si todas las piezas (de ambos jugadores) cumplen con esto, el juego termina en empate. */
			if(ocupante==jugador){
				origen.fil = i;
				origen.col = j;
				if(jugadaObligada(tablero, ocupante, origen, NULA)){
					*hayPaika=0;
					hayMovimientos=1;
				}
				/*Si puede comer en la jugada posterior, la casilla no esta bloqueada
				** (esta obligada a comer aunque sea mala estrategia)
				** si la jugada no es obligada, analizo para cada direccion si la captura es inminente en caso de dirigirse a ella*/
				else{
					dirsPosibles = tablero->matriz[i][j].tipo == FUERTE ? 8:4;
					for(n=0; n<dirsPosibles ; n++){
						dir = direcciones[n];
						incrementoSegunDir(&dirFil, &dirCol, dir);
															
				 #define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols)
				
					if( !FDERANGO(i+dirFil, j+dirCol) ){
						ady.fil=i+dirFil;
						ady.col=j+dirCol;
						if(tablero->matriz[ady.fil][ady.col].ocupante==VACIO && !meCapturan(tablero, ady, ocupante)) 
							hayMovimientos=1; /* Existe un movimiento en el que no se ve amenazado en la jugada posterior */
					}
					}
				}	
			#undef FDERANGO		
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

void cambiarTurno (int *jugador, tTablero * tablero, enum tDireccion * direccionPrevia) {
	*jugador = !(*jugador);
	limpiarTocadas(tablero);
	*direccionPrevia = NULA; /* Ninguna*/
}

int calcularMovCompu(tMovimiento * mov, tTablero * tablero, tFlag hayPaika, tFlag hayCadena, enum tDireccion direccionPrevia){

	tVecMovs movsPosibles;
	int i,j;
	int eleccion;
	tCoordenada casillaOrig;
        
	movsPosibles.dim = 0;
	movsPosibles.elems = NULL;

	if(!hayCadena){

		for(i=0 ; i<tablero->filas ; i++)
			for(j=0 ; j<tablero->cols ; j++)
				if(tablero->matriz[i][j].ocupante == NEGRO){
					casillaOrig.fil=i;
					casillaOrig.col=j;
					if( aumentarPosibles(tablero, &movsPosibles, casillaOrig, hayPaika, direccionPrevia) != 0)
						return 1;		
					}
	}
	else{
		/*Si estoy encadenando, la coordenada de origen esta ya determinada en mov que me pasan como parametro*/
		aumentarPosibles(tablero, &movsPosibles, mov->coordOrig, hayPaika, direccionPrevia);
	}

	eleccion = rand()%movsPosibles.dim;
	*mov = movsPosibles.elems[eleccion];
	return 0;
}

int guardarPartida(tTablero * tablero, int modo, int jugador, const char * nombre){
	FILE *f;
	int nfilas=tablero->filas;
	int ncols=tablero->cols;
	int i,j;
	
	jugador++; /*Debe ser 1 para el jugador 1 (que esta alojado como 0), y 2 para el jugador 2*/

	if((f=fopen(nombre, "wb")) == NULL)
		return ERROR;
	fwrite(&modo, sizeof(int),1,f);
	fwrite(&jugador,sizeof(int),1,f);
	fwrite(&nfilas, sizeof(int),1,f);
	fwrite(&ncols, sizeof(int),1,f);
	for(i=0; i<nfilas ; i++)
		for(j=0; j<ncols ; j++){
			switch(tablero->matriz[i][j].ocupante){
				case BLANCO:
				fputc('&', f); break;
				case NEGRO:
				fputc('#', f); break;
				case VACIO:
				fputc('0', f); break;
			}
		}	

	return 1;
}

tTablero cargarPartida(int * modo, int * jugador, const char * nombre){
	/*asumo que los datos estan validados, el archivo no esta corrputo*/
	int fils,cols;
	int i,j;
	FILE * f;
	tTablero tablero;

	if((f=fopen(nombre,"rb"))==NULL)
		{tablero.matriz=NULL;
		return tablero;
		}
	fread(modo, sizeof(int),1, f);
	fread(jugador, sizeof(int),1,f);
	fread(&fils, sizeof(int), 1, f);
	fread(&cols, sizeof(int), 1, f);

	(*jugador)--; /* En el savefile estan guardados como 1,2. Se manejan como 0,1*/
	

        tablero.matriz=generarMatrizTablero(fils, cols);
	if(tablero.matriz==NULL)
		return tablero;

	tablero.filas=fils;
	tablero.cols=cols;

	for(i=0; i<tablero.filas ; i++)
		for(j=0 ; j<tablero.cols ; j++){
			if (i%2 == j%2)
                        	tablero.matriz[i][j].tipo= FUERTE;
                	else
                        	 tablero.matriz[i][j].tipo= DEBIL;

			switch(fgetc(f)){
				case '0':
				tablero.matriz[i][j].ocupante=VACIO;
				break;	
				case '&':
				tablero.matriz[i][j].ocupante=BLANCO;
				break;	
				case '#':
				tablero.matriz[i][j].ocupante=NEGRO;
				break;	
				default:
				liberarTodo(tablero.matriz, tablero.filas);
				tablero.matriz=NULL;
				return tablero;
				break;
 			}
		}

	return tablero;
}

tTablero generarTablero(int fils, int cols){
	
	tTablero tablero;
	
	tablero.matriz=generarMatrizTablero(fils, cols);
	if(tablero.matriz == NULL)
		return tablero;

	tablero.filas=fils;
	tablero.cols=cols;
	
	rellenarTablero(&tablero);
	
	return tablero;

}

tCasilla ** generarMatrizTablero(int fils, int cols){

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

void copiarTablero(tTablero * tablero, tCasilla ** tableroAuxiliar){
	int i, j;
	for(i=0; i<tablero->filas ; i++)
		for(j=0 ; j<tablero->cols ; j++)
			tableroAuxiliar[i][j] = tablero->matriz[i][j];
}	

void intercambiarTableros(tTablero * tablero, tCasilla *** tableroAuxiliar){
	tCasilla ** aux;
	aux = *tableroAuxiliar;
	*tableroAuxiliar = tablero->matriz;
	tablero->matriz = aux;

}

static void rellenarTablero(tTablero * tablero){
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
				tablero->matriz[i][j].ocupante = postCentral ? BLANCO:NEGRO; /*No hay simetria con respecto a la central; se invierte la paridad luego de la misma*/
			else
				tablero->matriz[i][j].ocupante = postCentral ? NEGRO:BLANCO;
			
		}
		else{	/*Casilla central*/
			tablero->matriz[i][j].ocupante= VACIO;
			postCentral=1; /*Relevante para las otras piezas*/
		}	
	}	
	}
	
	return;
}

static void liberarTodo(tCasilla ** matriz, int n){
	
	int i;
	for(i=0; i<n ; i++)
		free(matriz[i]);
	
	free(matriz);
			  
}

static void limpiarTocadas(tTablero * tablero){
	int i,j;
	for(i=0; i<tablero->filas ; i++)
		for(j=0; j<tablero->cols ; j++)
			if(tablero->matriz[i][j].estado == TOCADA)
				tablero->matriz[i][j].estado = VACIO;
}

static void actualizarTablero(tTablero * tablero, enum tDireccion direccion, tMovimiento mov){
	int i, j;
	int dirFil, dirCol;
	int fini, cini;
	int jugador = tablero->matriz[mov.coordOrig.fil][mov.coordOrig.col].ocupante;
	int enemigo = !jugador;
	incrementoSegunDir(&dirFil, &dirCol, direccion);
	
	if(mov.tipoMov==WITHDRAWAL){
		fini=mov.coordOrig.fil - dirFil; /*De donde empieza a comer*/
		cini=mov.coordOrig.col - dirCol;
		dirFil = -dirFil;	/*Se invierte la direccion a recorrer*/
		dirCol = -dirCol;
	}

	else if(mov.tipoMov==APPROACH){
		fini=mov.coordDest.fil + dirFil; /*De donde empieza a comer*/
		cini=mov.coordDest.col + dirCol;
		}
	printf("%d,%d -> %d,%d\n", mov.coordOrig.fil+1, mov.coordOrig.col+1, mov.coordDest.fil+1, mov.coordDest.col+1);
	tablero->matriz[mov.coordDest.fil][mov.coordDest.col].ocupante = jugador;
	tablero->matriz[mov.coordOrig.fil][mov.coordOrig.col].ocupante = VACIO;	/*Movi la ficha*/

	i=fini;
	j=cini;
	while((i<(tablero->filas) && i>=0 && j<(tablero->cols) && j>=0) && tablero->matriz[i][j].ocupante==enemigo){
		/*Mientras no me vaya del tablero y siga habiendo enemigos en la linea de captura*/
		tablero->matriz[i][j].ocupante = VACIO;
		tablero->matriz[i][j].estado = LIBRE; /* Si vengo de cadena, estaba ACTIVA*/
		
		i+=dirFil;
		j+=dirCol;
	}
}

static void incrementoSegunDir(int * dirFil, int *dirCol, enum tDireccion direccion){

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

static int aumentarPosibles(tTablero * tablero, tVecMovs * movsPosibles, tCoordenada casillaOrig, tFlag hayPaika, enum tDireccion direccionPrevia){
        
	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	int dirsPosibles, dirFil, dirCol;	
	int n;
	tMovimiento * aux;
	enum tCaptura tipoComida;
	enum tDireccion dir;
	
	dirsPosibles = tablero->matriz[casillaOrig.fil][casillaOrig.col].tipo==FUERTE ? 8 : 4;
	
	for(n=0; n<dirsPosibles ; n++){
		dir = direcciones[n];

		if(dir != direccionPrevia){
			incrementoSegunDir(&dirFil, &dirCol, dir);
			#define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols)             
							
			if(!FDERANGO(casillaOrig.fil + dirFil, casillaOrig.col + dirCol) && tablero->matriz[casillaOrig.fil + dirFil][casillaOrig.col + dirCol].estado != TOCADA ){
				if( (tipoComida = hayComida(NEGRO, tablero, casillaOrig, dir)) != NINGUNO || hayPaika ){

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

					if(tipoComida == AMBOS) /*Elijo una al azar*/
						tipoComida = rand()%2?APPROACH:WITHDRAWAL;

					movsPosibles->elems[movsPosibles->dim].tipoMov = tipoComida;
		
					movsPosibles->dim++;
				}
			}
	}
    }

	return 0;
}

static enum tDireccion direccionDestino(tCoordenada origen, tCoordenada destino){

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

static int meCapturan(tTablero *tablero, tCoordenada posicion, int jugador){

	enum tDireccion direcciones[]={N,S,E,O,SE,SO,NE,NO};
	int dirsPosibles = tablero->matriz[posicion.fil][posicion.col].tipo == FUERTE ? 8:4;
	int i, dirFil, dirCol;
	enum tDireccion dir;
	tCoordenada adyacente, siguienteAdy;	
	int enemigo = !jugador;

	for(i=0; i<dirsPosibles ; i++){
		dir = direcciones[i];
		incrementoSegunDir(&dirFil, &dirCol, dir);
		
		/*Las casillas que pueden comerme por approach o widthraw en el proximo turno*/
		adyacente.fil = posicion.fil + dirFil;
		adyacente.col= posicion.col + dirCol;
		siguienteAdy.fil = adyacente.fil + dirFil;
		siguienteAdy.col = adyacente.col + dirCol;

        	#define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols)		
		
		if(!FDERANGO(adyacente.fil, adyacente.col) && !FDERANGO(siguienteAdy.fil, siguienteAdy.col)){

			if(tablero->matriz[adyacente.fil][adyacente.col].ocupante == enemigo
				&& tablero->matriz[siguienteAdy.fil][siguienteAdy.col].ocupante == VACIO){
					/*Me comen por WITHDRAWAL en la proxima jugada en esa direcicon*/
				printf("Si voy a %d, %d me capturan por withdrawal desde %d, %d\n",posicion.fil+1, posicion.col+1, adyacente.fil+1, adyacente.col+1);
					return 1;
			}	
			if(tablero->matriz[adyacente.fil][adyacente.col].ocupante == VACIO
				&& tablero->matriz[siguienteAdy.fil][siguienteAdy.col].ocupante == enemigo){
					/*Me comen por APPROACH en la proxima jugada, desde esa direccion*/
					printf("Si voy a %d, %d me capturan por approach desde %d, %d\n", posicion.fil+1, posicion.col+1, siguienteAdy.fil+1, siguienteAdy.col+1);
					return 1;
		}
		#undef FDERANGO
		}
	}

	printf("No me capturan desde %d, %d\n", posicion.fil, posicion.col);
	return 0; /*Si llegue hasta aca, no hay manera de que me capturen*/	

}

static enum tCaptura hayComida (int jugador, tTablero *tablero, tCoordenada origen, enum tDireccion direccion) { 

	/* Devuelve el tipo de captura posible segun el movimiento que quiera realizar el usuario
	** (direccion del movimiento desde de casilla origen). Si no encuentra, devuelve NINGUNO. 
	** Si la casilla a la que se quiere mover esta ocupada, devuelve NINGUNO (no devuelve error)
 	** Esto es porque si se me llama de validarMovimiento() esto ya esta validado,
 	** y si se me llama de paika() me interesa solo si se puede o no hacer el movimiento, 
	** no si el usuario ingreso algo valido */
	
	int fdA, fdW, cdA, cdW;
	int fueraDeRangoA, fueraDeRangoW;
	char fo = origen.fil;
        char co=origen.col;
        char enemigo = !jugador;
	enum tCaptura captura;

        int dirFil, dirCol;
        tFlag hayAppr=0, hayWithdr=0;
	
	incrementoSegunDir(&dirFil, &dirCol, direccion);
	
	fdA=fo+2*dirFil; /*Fila de la casilla a capturar por approach */
	cdA=co+2*dirCol; /*Columna*/
	fdW=fo-dirFil; /*Fila de la casilla a capturar por withdrawal */
	cdW=co-dirCol; /*Columna*/

	/* Reviso si los casilleros que tienen las fichas a capturar existen, y si la casilla a moverse esta vacia.
	** Si no, el movimiento es invalido (NINGUNO) */ 
		
	#define FDERANGO(x,y) ((x) < 0 || (y) < 0 || (x) >= tablero->filas || (y) >= tablero->cols) 
	
	fueraDeRangoA = ( FDERANGO(fdA,cdA) || FDERANGO(fo+dirFil, co+dirCol)
			|| tablero->matriz[fo+dirFil][co+dirCol].ocupante != VACIO);
	
	fueraDeRangoW = ((FDERANGO(fdW,cdW) || FDERANGO(fo+dirFil, co+dirCol))
			 || tablero->matriz[fo+dirFil][co+dirCol].ocupante != VACIO);
	#undef FDERANGO	

	if (!fueraDeRangoA && tablero->matriz[fdA][cdA].ocupante== enemigo)
                /*El elemento de la matriz corresponde a la casilla a capturar por approach*/
	       {hayAppr = 1;
		}
        if (!fueraDeRangoW && tablero->matriz[fdW][cdW].ocupante == enemigo)
                /*El elemento de la matriz corresponde a la casilla a capturar por withdrawal*/
                {hayWithdr = 1;
		 
		}

        if (hayAppr && hayWithdr)
                captura=AMBOS;
        else if (hayAppr)
                captura=APPROACH;
        else if (hayWithdr)
                captura=WITHDRAWAL;
        else
                captura=NINGUNO;

	return captura;
}

static int validarMovimiento(int jugador, tTablero * tablero, tMovimiento movimiento , enum tDireccion * direccionPrevia, tFlag hayPaika) {
	
	int jugada, aux;

	int fo=movimiento.coordOrig.fil;
	int co=movimiento.coordOrig.col;
	int fd=movimiento.coordDest.fil;
	int cd=movimiento.coordDest.col;
	
	enum tDireccion direccionMov;
/*	int hayPaika; */

	if(fd < 0 || fo < 0 || co < 0 || cd < 0 || fo >= tablero->filas || fd >= tablero->filas || co >= tablero->cols || cd >= tablero->cols)
		return ERR_MOV_RANGO; /*Fuera de limites del tablero*/

	if(jugador != tablero->matriz[fo][co].ocupante)
		return ERR_MOV_ORIG; /* No puede mover la ficha porque no es suya */

	if(tablero->matriz[fd][cd].ocupante != VACIO)
		return ERR_MOV_DEST; /* No puede mover porque la casilla no esta vacia */

	if(tablero->matriz[fd][cd].estado==TOCADA)
		return ERR_MOV_TOC;

	direccionMov = direccionDestino(movimiento.coordOrig, movimiento.coordDest);

	if(direccionMov == ERROR)
		return ERR_MOV_NO_ADY; /*No es una direccion valida*/

	if(tablero->matriz[fd][cd].tipo == DEBIL && (direccionMov > O)) /* NE, NO, SE, SO */  
		return ERR_MOV_DEBIL;

	if(direccionMov == *direccionPrevia)
		return ERR_MOV_DIR;		/*No puede moverse en la misma direccion en la que venia moviendose */	

	if( (aux=hayComida(jugador, tablero, movimiento.coordOrig, direccionMov)) != NINGUNO || hayPaika /*|| (hayPaika = paika(jugador, tablero))*/)
		/*Solamente chequeo la situacion de Paika si no selecciono un movimiento donde pueda comer */
		jugada=aux;
	else{
		printf("aux: %d, paika: %d", aux, hayPaika);
		return ERR_MOV_PAIKA;
	}
	
	/*Si llega hasta aca, no hay ningun error; actualizo el estado luego de que se efectue el movimiento*/
	
	tablero->matriz[fo][co].estado=TOCADA;
	
	*direccionPrevia = direccionMov;

	if(jugada==AMBOS && movimiento.tipoMov!=NINGUNO)
		/*Si el jugador puede hacer ambas cosas pero ya eligio*/
		jugada=movimiento.tipoMov;

	return jugada;
}

static int jugadaObligada(tTablero * tablero, int jugador, tCoordenada origen, enum tDireccion dirAnterior){

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

		if(tablero->matriz[destino.fil][destino.col].estado != TOCADA && direcciones[dir] != dirAnterior)
		 /*Solo chequeo si hay comida si no visite esa casilla antes y no debo ir en la misma dirección de antes */
			{ /*printf("%d, %d no esta tocada\n", destino.fil+1, destino.col+1)*/;
			if(hayComida(jugador, tablero, origen, direcciones[dir])!=NINGUNO)
			{/*	printf("Hay comida yendo a %d, %d\n", destino.fil+1, destino.col+1);*/
				return 1; /*Debe capturar esa pieza la proxima jugada */}
		}
			else
			;/*	printf("%d, %d esta tocada\n", destino.fil+1, destino.col+1);*/
		}
		}	

	return 0;
}

