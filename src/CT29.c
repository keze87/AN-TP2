// César Ezequiel Herrera 97429
// Numero de grupo: 29
//
// Repo con pruebas unitarias:
// https://github.com/keze87/AN-TP2
//
// Make:
// gcc -o CT29 CT29.c -lm -std=c11 -Wall -pedantic -pedantic-errors -I.

#include "tests.h" // Pruebas Unitarias

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define NUMERODEPADRON 97429

#define TRUE 0
#define FALSE 1

#define PI acos(-1.0)
#define STEFANBOLTZMANN 0.000000056703

void imprimirEnunciado (short enunciado) {

	switch (enunciado) {

		case 1:
			printf("\n1) Resolver el modelo planteado considerando solamente el intercambio de calor por convección.\n");
			printf("   Aplicar los métodos de Euler y Runge Kutta de orden 4 para una condición inicial T0=20°C y paso de tiempo h=cad.\n\n");
			break;

		case 2:
			printf("\n2) Resolver nuevamente el ítem 1 incorporando el término de intercambio radiativo.\n");
			printf("   a) Seleccionar el método que considere más adecuado de acuerdo a los resultados del ítem 1.\n");
			printf("   c) A partir de la evolución temporal obtenida en 2a) calcular los siguientes parámetros de salida del proceso:\n");
			printf("   Sk (Soaking) y Tsk (Temperatura promedio durante el soaking).\n\n");
			break;

		case 3:
			printf("\n3) Encontrar manualmente una combinación de T1 y T2 para que el soaking sea de 10 minutos manteniendo\n");
			printf("la temperatura objetivo Tsk obtenida en el ítem 2. La tolerancia para el soaking es de ±1min.\n\n");
			break;

		case 4:
			printf("\n4) Automatizar la búsqueda manual del ítem 3 para encontrar valores de T1 y T2 que obtengan un Tsk y Sk predeterminados.\n\n");
			break;

		default :
			printf("Error\n");

	}

}

/*
 *
 * Cálculos auxiliares
 *
 */

struct vectorDatos {

	int temperaturaInicial;

	// Propiedades del material
	int densidad;
	int calorEspecifico;

	// Geometría del material
	float diametroExterno;
	float espesor;
	int longitudTubo;

	// Geometría del horno
	int longitudHorno;
	int bolsillos;

	// Parámetros de proceso
	int cadencia;
	double temperaturaUno;
	double temperaturaDos;

	// Parámetros de la transferencia de calor
	int coeficienteConveccion;
	float factorEmisividad;

	// Auxiliares
	double masa;
	double superficie;
	float velocidad;
	int tiempoEnElHorno;

};


struct vectorDatos cargarVectorDatos () {

	struct vectorDatos aux;

	aux.temperaturaInicial		= 20 + 273;
	aux.densidad				= 7850;
	aux.calorEspecifico			= 480;
	aux.diametroExterno			= 244.48 / 1000;
	aux.espesor					=  13.84 / 1000;
	aux.longitudTubo			= 12;
	aux.longitudHorno			= 50;
	aux.bolsillos				= 50;
	aux.cadencia				= round( 35 - (float)  10 / 10000 * (NUMERODEPADRON - 90000));
	aux.temperaturaUno			= round(500 + (float) 200 / 10000 * (NUMERODEPADRON - 90000)) + 273;
	aux.temperaturaDos			= round(500 + (float) 200 / 10000 * (NUMERODEPADRON - 90000)) + 273;
	aux.coeficienteConveccion	= 20;
	aux.factorEmisividad		= 0.85;

	aux.masa			= aux.densidad * PI * aux.diametroExterno * aux.espesor \
							* (1 - aux.espesor / aux.diametroExterno) * aux.longitudTubo;

	aux.superficie		= PI * aux.diametroExterno * aux.longitudTubo;

	aux.velocidad		= (float) aux.longitudHorno / (aux.bolsillos * aux.cadencia);

	aux.tiempoEnElHorno	= aux.bolsillos * aux.cadencia;

	return aux;

}

int buscarEstabilidad (struct vectorDatos datos) {

	return datos.cadencia; // WONTFIX

}

// Devuelve un string del número redondeado
char * redondear (double numero, short cifrasSig) {

	char * aux = malloc(sizeof(char) * 31);
	char * auxInteger = malloc(sizeof(char) * 31);
	char auxModulo[30];

	long entero = round(numero);

	snprintf(auxInteger, 30, "%li", entero);
	snprintf(auxModulo, 30, "%li", labs(entero));

	// Si es mayor o igual (en modulo) a 100 devuelvo solo la parte entera
	// Sino devuelvo un decimal o 2
	if (strlen(auxModulo) >= cifrasSig) {

		free(aux);

		return auxInteger;

	} else {

		// ejemplo: formato = "%.5f"
		char formato[10];
		char cantDecimales[10];

		sprintf(cantDecimales, "%d", cifrasSig - (int) strlen(auxModulo));

		strcpy(formato, "%.");
		strcat(formato, cantDecimales);
		strcat(formato, "f");

		snprintf(aux, 30, formato, numero);

	}

	free(auxInteger);

	return aux;

}

// PRE: decimales después del punto
char * incerteza (char * raiz) {

	char * punteroADecimalDespuesDePunto = NULL;
	char * retorno = malloc(sizeof(char) * 30);
	int cantidadDeDecimales;

	// el puntero es igual a la posición de memoria del caracter después del punto
	// "123.456" -> "456"
	for (int i = 0; i < strlen(raiz); i++) {

		if (raiz[i] == '.') {

			punteroADecimalDespuesDePunto = raiz + i + 1;

			break;

		}

	}

	if (punteroADecimalDespuesDePunto == NULL) {

		strcpy(retorno, "1");

		return retorno;

	}

	cantidadDeDecimales = strlen(punteroADecimalDespuesDePunto);

	strcpy(retorno, "0.");

	for (int j = 1; j < cantidadDeDecimales; j++)
		strcat(retorno,"0");

	strcat(retorno,"1");

	return retorno;

}

// segAMinutos(60) -> 1 m 0 s
char * segAMinutos (int segundos) {

	char auxM[40];
	char auxR[40];

	int minutos = (int) segundos / 60;
	int resto = (int) segundos - minutos * 60;

	snprintf(auxM, 40, "%d", minutos);
	snprintf(auxR, 40, "%d", resto);

	char * retorno = malloc(sizeof(char) * (10 + strlen(auxM) + strlen(auxR)));

	strcpy(retorno, "");

	if (minutos < 10)
		strcat(retorno, " ");

	strcat(retorno, auxM);
	strcat(retorno, " m ");

	if (resto < 10)
		strcat(retorno, " ");

	strcat(retorno, auxR);

	strcat(retorno, " s");

	return retorno;

}

/*
 *
 * Manejo de lista
 *
 */

struct elementoLista {

	int t;		// Tiempo
	double T;	// Temperatura

};

// https://github.com/fiuba-7541/elemental ?
/*
 * Movimientos que va a manejar la estructura. Son de conocimiento público,
 * pero sólo deberían usarse para el manejo puntual de esta estructura.
 */
typedef enum {

	L_Primero,
	L_Siguiente,
	L_Anterior

} TMovimiento_Ls;

/*
 * Estructura auxiliar de la lista simple. Es privada y no debe usarse bajo
 * ningún concepto en la aplicación.
 */
typedef struct TNodoListaSimple {

	void * Elem;
	struct TNodoListaSimple * Siguiente, * Anterior;

} TNodoListaSimple;

/**
 * Estructura cabecera, este es el tipo que se deberá instanciar, aunque
 * nunca acceder a sus campos.
 */
typedef struct {

	TNodoListaSimple * Primero, * Corriente;
	int TamanioDato;

} TListaSimple;

/**
 * L_CREAR
 * Pre: Ls no fue creada.
 * Post: Ls creada y vacia
 */
void L_Crear (TListaSimple * pLs, int TamanioDato) {

	pLs->Corriente = NULL;
	pLs->Primero = NULL;
	pLs->TamanioDato = TamanioDato;

}

/**
 * L_VACIAR
 * Pre: Ls creada.
 * Post: Ls vacia.
 */
void L_Vaciar (TListaSimple * pLs) {

	TNodoListaSimple * pNodo, * Siguiente;

	for(pNodo = pLs->Primero; (pNodo); pNodo = Siguiente) {

		Siguiente = pNodo->Siguiente;

		free(pNodo->Elem);
		free(pNodo);

	}

	pLs->Primero = pLs->Corriente = NULL;

}

/**
 * L_VACIA
 * Pre: Ls creada.
 * Post: Si Ls tiene elementos devuelve FALSE sino TRUE.
 */
int L_Vacia (TListaSimple Ls) {

	if (Ls.Primero == NULL)
		return TRUE;

	return FALSE;

}

/**
 * L_ELEM_CTE
 * Pre: Ls creada y no vacia.
 * Post: Se devuelve en E el elemento Corriente de la lista.
 */
void L_Elem_Cte (TListaSimple Ls, void * pE) {

	memcpy(pE, Ls.Corriente->Elem, Ls.TamanioDato);

}

/**
 * L_MOVER_CTE
 * Pre: Ls creada y no vacia.
 * Post: Si Ls esta vacia, devuelve FALSE. Sino:
 * Si M = L_Primero, el nuevo elemento Corriente es el Primero. Devuelve TRUE
 * Si M = L_Siguiente, el nuevo elemento Corriente es el Siguiente al
 * Corriente. Si estaba en el ultimo elemento, devuelve FALSE, sino TRUE.
 * Si M = L_Anterior, el nuevo elemento Corriente es el Anterior al
 * Corriente. Si estaba en el primer elemento, devuelve FALSE, sino TRUE.
 */
int L_Mover_Cte (TListaSimple * pLs, TMovimiento_Ls M) {

	switch (M) {

		case L_Primero:
			pLs->Corriente = pLs->Primero;
			break;

		case L_Siguiente:
			if (pLs->Corriente->Siguiente == NULL)
				return FALSE;
			else
				pLs->Corriente = pLs->Corriente->Siguiente;
			break;

		case L_Anterior:
			if (pLs->Corriente->Anterior == NULL)
				return FALSE;
			else
				pLs->Corriente = pLs->Corriente->Anterior;
			break;

	}

	return TRUE;

}

/**
 * L_INSERTAR_CTE
 * Pre: Ls creada.
 * Post: E se agrego a la lista y es el actual elemento Corriente.
 * Si M=L_Primero: se inserto como Primero de la lista.
 * Si M=L_Siguiente: se inserto despues del elemento Corriente.
 * Si M=L_Anterior: se inserto antes del elemento Corriente.
 * Si pudo insertar el elemento devuelve TRUE, sino FALSE.
 */
int L_Insertar_Cte (TListaSimple * pLs, TMovimiento_Ls M, void * pE) {

	TNodoListaSimple * pNodo = (TNodoListaSimple *) malloc(sizeof(TNodoListaSimple));

	if (!pNodo)
		return FALSE; // No hay memoria disponible

	pNodo->Elem = malloc(pLs->TamanioDato);

	if (!pNodo->Elem) {

		free(pNodo);

		return FALSE;

	}

	memcpy(pNodo->Elem, pE, pLs->TamanioDato);

	if ((pLs->Primero == NULL) || (M == L_Primero) || ((M == L_Anterior) && (pLs->Primero == pLs->Corriente))) {

		/*Si está vacía o hay que insertar en el Primero o
		hay que insertar en el Anterior y el actual es el Primero */
		pNodo->Siguiente = pLs->Primero;
		if (pLs->Primero != NULL)
			pLs->Primero->Anterior = pNodo;
		pLs->Primero = pLs->Corriente = pNodo;

	} else {

		// Siempre inserto como siguiente, el nodo nuevo, porque es más fácil
		pNodo->Siguiente = pLs->Corriente->Siguiente;
		pNodo->Anterior = pLs->Corriente;
		if (pLs->Corriente->Siguiente != NULL)
			pLs->Corriente->Siguiente->Anterior = pNodo;
		pLs->Corriente->Siguiente = pNodo;

		if (M == L_Anterior) {

			// Pero cuando el movimiento es Anterior, entonces swapeo los elementos
			void * tmp = pNodo->Elem;
			pNodo->Elem = pLs->Corriente->Elem;
			pLs->Corriente->Elem = tmp;

		}

	}

	pLs->Corriente = pNodo;

	return TRUE;

}

// Creo lista con elemento inicial T(0) = t0
TListaSimple crearListaVI (double valorInicial) {

	TListaSimple lista;
	L_Crear(& lista, sizeof(struct elementoLista));

	struct elementoLista vInicial;
	vInicial.t = 0;
	vInicial.T = valorInicial;

	L_Insertar_Cte(& lista, L_Primero, & vInicial);

	return lista;

}

// Imprime lista a pantalla
void imprimirLista (TListaSimple lista) {

	if (L_Vacia(lista) == FALSE) {

		int retorno = L_Mover_Cte(& lista, L_Primero);

		struct elementoLista elem;

		while (retorno == TRUE) {

			L_Elem_Cte(lista, & elem);

			char * auxTiempo = segAMinutos(elem.t);
			char * auxTemp = redondear(elem.T - 273, 3);
			char * auxError = incerteza(auxTemp);

			printf("T(%s) = %s ± %s ºC\n", auxTiempo, auxTemp, auxError);

			free(auxTiempo); free(auxTemp); free(auxError);

			retorno = L_Mover_Cte(&lista, L_Siguiente);

		}

	}

}

/*
 *
 * Funciones
 *
 */

double fConveccion (float t, double Tn, struct vectorDatos d /* datos */) {

	int Tinfinito;

	if (t * d.velocidad > d.longitudHorno / 2)

		Tinfinito = d.temperaturaDos;

	else

		Tinfinito = d.temperaturaUno;

	return - d.coeficienteConveccion * d.superficie * (Tn - Tinfinito) / (d.masa * d.calorEspecifico);

}

double fConveccionRadiaccion (float t, double Tn, struct vectorDatos d /* datos */) {

	int Tinfinito;

	if (t * d.velocidad > d.longitudHorno / 2)

		Tinfinito = d.temperaturaDos;

	else

		Tinfinito = d.temperaturaUno;

	return (d.coeficienteConveccion * d.superficie * (Tn - Tinfinito) \
			+ STEFANBOLTZMANN * d.factorEmisividad * d.superficie * (pow(Tn, 4) - pow(Tinfinito, 4))) \
			/ (- d.masa * d.calorEspecifico);

}

/*
 *
 * Métodos
 *
 */

// PRE: Lista no vacia
void euler (double (* funcion)(float, double, struct vectorDatos), TListaSimple * lista, int h, struct vectorDatos datos) {

	struct elementoLista n;
	L_Elem_Cte(* lista, &n);

	while (n.t < datos.tiempoEnElHorno) {

		struct elementoLista n1;
		n1.t = n.t + h;
		n1.T = n.T + h * funcion(n.t, n.T, datos);

		L_Insertar_Cte(lista, L_Siguiente, & n1);

		n.t = n1.t;
		n.T = n1.T;

	}

}

// PRE: Lista no vacia
void rungeKutta (double (* funcion)(float, double, struct vectorDatos), TListaSimple * lista, int h, struct vectorDatos datos) {

	struct elementoLista n;
	L_Elem_Cte(* lista, & n);

	while (n.t < datos.tiempoEnElHorno) {

		double k1 = funcion(n.t, n.T,								datos);
		double k2 = funcion((float) n.t + h / 2, n.T + h * k1 / 2,	datos);
		double k3 = funcion((float) n.t + h / 2, n.T + h * k2 / 2,	datos);
		double k4 = funcion((float) n.t + h, n.T + h * k3,			datos);

		struct elementoLista n1;
		n1.t = n.t + h;
		n1.T = n.T + h * (k1 / 6 + k2 / 3 + k3 / 3 + k4 / 6);

		L_Insertar_Cte(lista, L_Siguiente, &n1);

		n.t = n1.t;
		n.T = n1.T;

	}

}

/*
 *
 * Main
 *
 */

int buscarSk (TListaSimple * lista, short quiet) {

	if (L_Vacia(* lista) == TRUE)
		return FALSE;

	int retorno = L_Mover_Cte(lista, L_Siguiente);

	// Busco la temperatura final
	while (retorno == TRUE)
		retorno = L_Mover_Cte(lista, L_Siguiente);

	struct elementoLista final;
	L_Elem_Cte(* lista, & final);

	// Empiezo a recorrer la lista
	struct elementoLista soaking;
	struct elementoLista aux;

	retorno = L_Mover_Cte(lista, L_Anterior);

	while (retorno == TRUE) {

		L_Elem_Cte(* lista, & aux);

		if (aux.T >= final.T - 10) {

			soaking.T = aux.T;
			soaking.t = aux.t;

		} else {

			retorno = L_Mover_Cte(lista, L_Siguiente); // Pongo el cte en Sk
			L_Elem_Cte(* lista, & aux);

			break;

		}

		retorno = L_Mover_Cte(lista, L_Anterior);

	}

	if (quiet != TRUE) {

		char * auxSk = segAMinutos(final.t - soaking.t);

		printf("\nSk = %s\n", auxSk);

		free(auxSk);

	}

	return final.t - soaking.t;

}

double buscarTsk (int Sk, TListaSimple * lista, short quiet) {

	if ((L_Vacia(* lista) == TRUE) || (Sk == FALSE))
		return FALSE;

	int retorno = TRUE;

	struct elementoLista aux;
	L_Elem_Cte(* lista, & aux);

	// Me aseguro que el corriente está en el sk
	if (aux.t != Sk) {

		retorno = L_Mover_Cte(lista, L_Primero);

		while (retorno == TRUE) {

			L_Elem_Cte(* lista, & aux);

			if (aux.t == Sk)
				break;

			retorno = L_Mover_Cte(lista, L_Siguiente);

		}

	}

	int n = 0;
	double Tsk = 0;

	while (retorno == TRUE) {

		L_Elem_Cte(* lista, & aux);

		Tsk = Tsk + aux.T;
		n++;

		retorno = L_Mover_Cte(lista, L_Siguiente);

	}

	Tsk = Tsk / n;

	if (quiet != TRUE) {

		char * auxRedondeo = redondear(Tsk - 273, 4);
		char * auxError = incerteza(auxRedondeo);

		printf("\nTsk = %s ± %s ºC\n", auxRedondeo, auxError);

		free(auxRedondeo); free(auxError);

	}

	return Tsk;

}

void resolverConveccion (struct vectorDatos datos) {

	printf("Método de Euler:\n\n");

	TListaSimple lista = crearListaVI(datos.temperaturaInicial);

	int h = buscarEstabilidad(datos);

	euler(fConveccion, & lista, h, datos);

	imprimirLista(lista);

	L_Vaciar(& lista);


	printf("\nMétodo de Runge Kutta:\n\n");

	lista = crearListaVI(datos.temperaturaInicial);

	h = buscarEstabilidad(datos);

	rungeKutta(fConveccion, & lista, h, datos);

	imprimirLista(lista);

	L_Vaciar(& lista);

}

void resolverConveccionYRadiaccion (struct vectorDatos datos) {

	TListaSimple lista;
	int h;

	printf("\nMétodo de Runge Kutta:\n\n");

	lista = crearListaVI(datos.temperaturaInicial);

	h = buscarEstabilidad(datos);

	// euler(fConveccionRadiaccion, & lista, h, datos);
	rungeKutta(fConveccionRadiaccion, & lista, h, datos);

	imprimirLista(lista);

	int Sk = buscarSk(& lista, FALSE);
	buscarTsk(Sk, & lista, FALSE);

	L_Vaciar(& lista);

}

void buscarManualmenteT1YT2 (struct vectorDatos d) {

	d.temperaturaUno = 724 + 273;
	d.temperaturaDos = 635 + 273;

	printf("Pruebo con T1 = %.1F ºC y T2 = %.1F ºC\n", d.temperaturaUno - 273, d.temperaturaDos - 273);

	resolverConveccionYRadiaccion(d);

}

// skobj [segundos] y tskobj [ºK] ambos positivos
void buscarT1YT2 (int skobj, int tskobj) {

	char * aux = segAMinutos(skobj);
	printf("*) Skobj = %s, Tskobj = %d ºC\n", aux, tskobj - 273);
	free(aux);

	int n = 0;

	double T1n1;
	double T2n1;

	double T1n = 630 + 273;
	double T2n = 630 + 273;

	TListaSimple lista;

	struct vectorDatos datos = cargarVectorDatos();

	int h = datos.cadencia / 2;

	double f2menos1;
	double f2 = -22222;

	while (n < 100) {

		datos.temperaturaUno = T1n;
		datos.temperaturaDos = T2n;
		lista = crearListaVI(datos.temperaturaInicial);

		rungeKutta(fConveccionRadiaccion, & lista, h, datos);

		int f1 = buscarSk(& lista, TRUE);
		f2menos1 = f2;
		f2 = buscarTsk(f1, & lista, TRUE);

		L_Vaciar(& lista);

		double T1n1menos1 = T1n1;
		double T2n1menos1 = T2n1;

		T1n1 = T1n - 0.1 * (f1 - skobj) - 0.01 * (f2 - tskobj);
		T2n1 = T2n - 0.01 * (f1 - skobj) - 0.1 * (f2 - tskobj);

		T1n = T1n1;
		T2n = T2n1;

		// Solo necesario poque oscila con este jacobiano
		if (fabs(f2menos1 - tskobj) < fabs(f2 - tskobj)) {

			T1n1 = T1n1menos1;
			T2n1 = T2n1menos1;

		}

		n++;

	}

	char * auxRedondeo = redondear(T1n1 - 273, 3);
	char * auxError = incerteza(auxRedondeo);

	printf("\nT1 = %s ± %s ºC", auxRedondeo, auxError);

	free(auxRedondeo); free(auxError);

	auxRedondeo = redondear(T2n1 - 273, 3);
	auxError = incerteza(auxRedondeo);

	printf(", T2 = %s ± %s ºC\n\n", auxRedondeo, auxError);

	free(auxRedondeo); free(auxError);

}

int proceso () {

	struct vectorDatos datos = cargarVectorDatos();

	imprimirEnunciado(1);
	resolverConveccion(datos);

	imprimirEnunciado(2);
	resolverConveccionYRadiaccion(datos);

	imprimirEnunciado(3);
	buscarManualmenteT1YT2(datos);

	imprimirEnunciado(4);
	buscarT1YT2(600, 630 + 273);
	buscarT1YT2(600, round((float) 100 / 10000 * (NUMERODEPADRON - 90000) + 550) + 273);
	buscarT1YT2(600, round((float) 100 / 10000 * (NUMERODEPADRON - 90000) + 600) + 273);

	return TRUE;

}

int main () {

	int aux = TRUE;

	aux = correrTests(); // Pruebas Unitarias

	if (aux == TRUE)
		return proceso();

	return aux;

}

