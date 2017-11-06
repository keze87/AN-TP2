
#include "tests.h" // Pruebas Unitarias

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <memory.h>

#define NUMERODEPADRON 97429

#define TRUE 0
#define FALSE 1

#define PI acos(-1.0)
#define STEFANBOLTZMANN 0.000000056703

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
	int temperaturaUno;
	int temperaturaDos;

	// Parámetros de la transferencia de calor
	int coeficienteConveccion;
	float factorEmisividad;

	// Auxiliares
	double masa;
	double superficie;
	float velocidad;
	int tiempoEnElHorno;

};

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

struct elementoLista {

	int t;
	double T;

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

void L_Crear (TListaSimple * pLs, int TamanioDato) {

	pLs->Corriente = NULL;
	pLs->Primero = NULL;
	pLs->TamanioDato = TamanioDato;

}

void L_Vaciar (TListaSimple * pLs) {

	TNodoListaSimple * pNodo, * Siguiente;

	for(pNodo = pLs->Primero; (pNodo); pNodo = Siguiente) {

		Siguiente = pNodo->Siguiente;

		free(pNodo->Elem);
		free(pNodo);

	}

	pLs->Primero = pLs->Corriente = NULL;

}

int L_Vacia (TListaSimple Ls) {

	if (Ls.Primero == NULL)
		return TRUE;

	return FALSE;

}

void L_Elem_Cte (TListaSimple Ls, void * pE) {

	memcpy(pE, Ls.Corriente->Elem, Ls.TamanioDato);

}

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
	aux.cadencia				= round( 35 -  10 / 10000 * (NUMERODEPADRON - 90000));
	aux.temperaturaUno			= round(500 + 200 / 10000 * (NUMERODEPADRON - 90000)) + 273;
	aux.temperaturaDos			= round(500 + 200 / 10000 * (NUMERODEPADRON - 90000)) + 273;
	aux.coeficienteConveccion	= 20;
	aux.factorEmisividad		= 0.85;

	aux.masa			= aux.densidad * PI * aux.diametroExterno * aux.espesor \
							* (1 - aux.espesor / aux.diametroExterno) * aux.longitudTubo;

	// 2πh(r1+r2) + 2π(r1^2-r2^2)
	aux.superficie		= 2 * PI * (aux.diametroExterno / 2 * (aux.longitudTubo + aux.diametroExterno / 2) \
							+ (aux.diametroExterno / 2 - aux.espesor) \
							* (aux.longitudTubo - (aux.diametroExterno / 2 - aux.espesor)));

	aux.velocidad		= (float) aux.longitudHorno / (aux.bolsillos * aux.cadencia);

	aux.tiempoEnElHorno	= aux.bolsillos * aux.cadencia;

	return aux;

}

int buscarEstabilidad (struct vectorDatos datos) {

	return 1; //TODO

}

double fConveccion (int t, double Tn, struct vectorDatos d /* datos */) {

	int Tinfinito;

	if (t * d.velocidad > d.longitudHorno / 2)

		Tinfinito = d.temperaturaDos;

	else

		Tinfinito = d.temperaturaUno;

	return - d.coeficienteConveccion * d.superficie * (Tn - Tinfinito) / (d.masa * d.calorEspecifico);

}

// PRE: Lista no vacia
void euler (double (* funcion)(int, double, struct vectorDatos), TListaSimple * lista, int h, struct vectorDatos datos) {

	struct elementoLista n;
	L_Elem_Cte(* lista, &n);

	struct elementoLista n1;
	n1.t = n.t + h;
	n1.T = n.T + h * funcion(n.t, n.T, datos);

	L_Insertar_Cte(lista, L_Siguiente, & n1);

	if (n1.t < datos.tiempoEnElHorno)
		euler(funcion, lista, h, datos);

}

void rungeKutta (double (* funcion)(int, double, struct vectorDatos), TListaSimple * lista, int h, struct vectorDatos datos) {

	struct elementoLista n;
	L_Elem_Cte(* lista, & n);

	double k1 = funcion(n.t, n.T,						datos);
	double k2 = funcion(n.t + h / 2, n.T + h * k1 /2,	datos);
	double k3 = funcion(n.t + h / 2, n.T + h * k2 /2,	datos);
	double k4 = funcion(n.t + h, n.T + h * k3,			datos);

	struct elementoLista n1;
	n1.t = n.t + h;
	n1.T = n.T + h * (k1 / 6 + k2 / 3 + k3 / 3 + k4 / 6);

	L_Insertar_Cte(lista, L_Siguiente, &n1);

	if (n1.t < datos.tiempoEnElHorno)
		rungeKutta(funcion, lista, h, datos);

}

void imprimirLista (TListaSimple lista) {

	if (L_Vacia(lista) == FALSE) {

		int retorno = L_Mover_Cte(& lista, L_Primero);

		struct elementoLista elem;

		while (retorno == TRUE) {

			L_Elem_Cte(lista, & elem);

			if ((float) elem.t / 60 == (int) (elem.t / 60)) // Solo imprimo cada 1 minuto
				printf("T(%d) = %.3F\n", (int) elem.t / 60, elem.T - 273);

			retorno = L_Mover_Cte(&lista, L_Siguiente);

		}

		if ((float) elem.t / 60 != (int) (elem.t / 60)) // Si no imprimí el ultimo, lo imprimo
			printf("T(%d+) = %.3F\n", (int) elem.t / 60, elem.T - 273);

	}

}

TListaSimple crearListaVI (double valorInicial) {

	TListaSimple lista;
	L_Crear(& lista, sizeof(struct elementoLista));

	struct elementoLista vInicial;
	vInicial.t = 0;
	vInicial.T = valorInicial;

	L_Insertar_Cte(& lista, L_Primero, & vInicial);

	return lista;

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

	L_Vaciar(&lista);

}

double fConveccionRadiaccion (int t, double Tn, struct vectorDatos d /* datos */) {

	int Tinfinito;

	if (t * d.velocidad > d.longitudHorno / 2)

		Tinfinito = d.temperaturaDos;

	else

		Tinfinito = d.temperaturaUno;

	return (d.coeficienteConveccion * d.superficie * (Tn - Tinfinito) \
			+ STEFANBOLTZMANN * d.factorEmisividad * d.superficie * (pow(Tn, 4) - pow(Tinfinito, 4))) \
			/ (- d.masa * d.calorEspecifico);

}

int buscarSoaking (TListaSimple * lista) {

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

	printf("\nSk = %d +/- 1 minutos\n", (int) soaking.t / 60);

	if ((aux.t != soaking.t) || (aux.T != soaking.T) || (retorno != TRUE))
		return FALSE;

	int n = 0;
	double Tsk = 0;

	while (retorno == TRUE) {

		L_Elem_Cte(* lista, & aux);

		Tsk = Tsk + aux.T;
		n++;

		retorno = L_Mover_Cte(lista, L_Siguiente);

	}

	printf("\nTsk = %.3F +/-  ºK\n", (Tsk / n) - 273); //TODO

	return TRUE;

}

void resolverConveccionYRadiaccion (struct vectorDatos datos) {

	TListaSimple lista;
	int h;

	/* printf("Método de Euler:\n\n");
	lista = crearListaVI(datos.temperaturaInicial);
	h = buscarEstabilidad(datos);
	euler(fConveccionRadiaccion, &lista, h, datos);
	imprimirLista(lista);
	L_Vaciar(&lista); */

	printf("\nMétodo de Runge Kutta:\n\n");

	lista = crearListaVI(datos.temperaturaInicial);

	h = buscarEstabilidad(datos);

	rungeKutta(fConveccionRadiaccion, &lista, h, datos);

	imprimirLista(lista);

	buscarSoaking(& lista);

	L_Vaciar(& lista);

}

void buscarManualmenteT1YT2 (struct vectorDatos datos) {

	printf("Pruebo con T1 = 536 ºC y T2 = 489 ºC\n");

	datos.temperaturaUno = 536 + 273;
	datos.temperaturaDos = 489 + 273;

	resolverConveccionYRadiaccion(datos);

}

int proceso () {

	struct vectorDatos datos = cargarVectorDatos ();

	imprimirEnunciado(1);
	resolverConveccion(datos);

	imprimirEnunciado(2);
	resolverConveccionYRadiaccion(datos);

	imprimirEnunciado(3);
	buscarManualmenteT1YT2(datos);

	//imprimirEnunciado(4);


	return TRUE;

}

int main () {

	int aux = TRUE;

	aux = correrTests(); // Pruebas Unitarias

	if (aux == TRUE)
		return proceso();

	return aux;

}

