
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
			printf("la temperatura objetivo T sk obtenida en el ítem 2. La tolerancia para el soaking es de ±1min.\n\n");
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
/**
 * Movimientos que va a manejar la estructura. Son de conocimiento público,
 * pero sólo deberían usarse para el manejo puntual de esta estructura.
 */
typedef enum {

	L_Primero,
	L_Siguiente,
	L_Anterior

} TMovimiento_Ls;

/**
 * Estructura auxiliar de la lista simple. Es privada y no debe usarse bajo
 * ningún concepto en la aplicación.
 */
typedef struct TNodoListaSimple {

	void* Elem;
	struct TNodoListaSimple *Siguiente;

} TNodoListaSimple;

/**
 * Estructura cabecera, este es el tipo que se deberá instanciar, aunque
 * nunca acceder a sus campos.
 */
typedef struct {

	TNodoListaSimple *Primero, *Corriente;
	int TamanioDato;

} TListaSimple;

void L_Crear (TListaSimple *pLs, int TamanioDato) {

	pLs->Corriente = NULL;
	pLs->Primero = NULL;
	pLs->TamanioDato = TamanioDato;

}

void L_Vaciar (TListaSimple *pLs) {

	TNodoListaSimple *pNodo, *Siguiente;

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

void L_Elem_Cte (TListaSimple Ls, void *pE) {

	memcpy(pE, Ls.Corriente->Elem, Ls.TamanioDato);

}

int L_Mover_Cte (TListaSimple *pLs, TMovimiento_Ls M) {

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

		case L_Anterior:return FALSE;

	}

	return TRUE;

}

int L_Insertar_Cte (TListaSimple *pLs, TMovimiento_Ls M, void* pE) {

	TNodoListaSimple *pNodo = (TNodoListaSimple*) malloc(sizeof(TNodoListaSimple));

	if (!pNodo)
		return FALSE; /* No hay memoria disponible */

	pNodo->Elem = malloc (pLs->TamanioDato);

	if (!pNodo->Elem) {

		free(pNodo);

		return FALSE;

	}

	memcpy(pNodo->Elem, pE, pLs->TamanioDato);

	if ((pLs->Primero == NULL) || (M == L_Primero) || ((M == L_Anterior) && (pLs->Primero == pLs->Corriente))) {

		/*Si está vacía o hay que insertar en el Primero o
		hay que insertar en el Anterior y el actual es el Primero */
		pNodo->Siguiente = pLs->Primero;
		pLs->Primero = pLs->Corriente = pNodo;

	} else {

		/* Siempre inserto como siguiente, el nodo nuevo, porque es más fácil */
		pNodo->Siguiente = pLs->Corriente->Siguiente;
		pLs->Corriente->Siguiente = pNodo;

		if (M == L_Anterior) {

			/* Pero cuando el movimiento es Anterior, entonces swapeo los
			 * elementos */
			void* tmp = pNodo->Elem;
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

	aux.velocidad		= aux.longitudHorno / (aux.bolsillos * aux.cadencia);

	aux.tiempoEnElHorno	= aux.bolsillos * aux.cadencia;

	return aux;

}

int buscarEstabilidad (struct vectorDatos datos) {

	return 1; //TODO

}

double fConveccion (double Tn, struct vectorDatos datos) {

	int Tinfinito;

	if (Tn * datos.velocidad > datos.longitudHorno / 2)

		Tinfinito = datos.temperaturaDos;

	else

		Tinfinito = datos.temperaturaUno;

	return - datos.coeficienteConveccion * datos.superficie * (Tn - Tinfinito) / (datos.masa * datos.calorEspecifico);

}

// PRE: Lista no vacia
void euler (TListaSimple * lista, int h, struct vectorDatos datos) {

	struct elementoLista n;
	L_Elem_Cte(*lista, &n);

	struct elementoLista n1;
	n1.t = n.t + h;
	n1.T = n.T + h * fConveccion(n.T, datos);

	L_Insertar_Cte(lista, L_Siguiente, &n1);

	if (n1.t < datos.tiempoEnElHorno)
		euler(lista, h, datos);

}

void rungeKutta (TListaSimple * lista, int h, struct vectorDatos datos) {

	struct elementoLista n;
	L_Elem_Cte(*lista, &n);

	double k1 = fConveccion(n.T,				datos);
	double k2 = fConveccion(n.T + h * k1 /2,	datos);
	double k3 = fConveccion(n.T + h * k2 /2,	datos);
	double k4 = fConveccion(n.T + h * k3,		datos);

	struct elementoLista n1;
	n1.t = n.t + h;
	n1.T = n.T + h * (k1 / 6 + k2 / 3 + k3 / 3 + k4 / 6);

	L_Insertar_Cte(lista, L_Siguiente, &n1);

	if (n1.t < datos.tiempoEnElHorno)
		rungeKutta(lista, h, datos);

}

void imprimirLista (TListaSimple lista) {

	if (L_Vacia(lista) == FALSE) {

		int retorno = L_Mover_Cte(&lista, L_Primero);

		struct elementoLista elem;

		while (retorno == TRUE) {

			L_Elem_Cte(lista, &elem);

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
	L_Crear(&lista, sizeof(struct elementoLista));

	struct elementoLista vInicial;
	vInicial.t = 0;
	vInicial.T = valorInicial;

	L_Insertar_Cte(&lista, L_Primero, &vInicial);

	return lista;

}

int resolverConveccion () {

	struct vectorDatos datos = cargarVectorDatos();

	// Euler
	TListaSimple lista = crearListaVI(datos.temperaturaInicial);

	int h = buscarEstabilidad(datos);

	euler(&lista, h, datos);

	printf("Método de Euler:\n\n");

	imprimirLista(lista);

	L_Vaciar(&lista);

	// RK
	lista = crearListaVI(datos.temperaturaInicial);

	h = buscarEstabilidad(datos);

	rungeKutta(&lista, h, datos);

	printf("\nMétodo de Runge Kutta:\n\n");

	imprimirLista(lista);

	L_Vaciar(&lista);

	return TRUE;

}

int proceso () {

	imprimirEnunciado(1);
	resolverConveccion();

	imprimirEnunciado(2);


	//imprimirEnunciado(3);


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

