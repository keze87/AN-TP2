
#include <stdio.h>
#include <math.h>

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

struct elementoLista {

	int t;
	double T;

};

typedef enum {

	L_Primero,
	L_Siguiente,
	L_Anterior

} TMovimiento_Ls;

typedef struct TNodoListaSimple {

	void* Elem;
	struct TNodoListaSimple * Siguiente, * Anterior;

} TNodoListaSimple;

typedef struct {

	TNodoListaSimple * Primero, * Corriente;
	int TamanioDato;

} TListaSimple;

int L_Vacia(TListaSimple Ls);
void L_Elem_Cte (TListaSimple Ls, void * pE);
int L_Mover_Cte (TListaSimple * pLs, TMovimiento_Ls M);
void L_Vaciar (TListaSimple * pLs);

struct vectorDatos cargarVectorDatos ();
TListaSimple crearListaVI (double valorInicial);
void euler (double (* funcion)(double, struct vectorDatos), TListaSimple * lista, int h, struct vectorDatos datos);
void rungeKutta (double (* funcion)(double, struct vectorDatos), TListaSimple * lista, int h, struct vectorDatos datos);
double fConveccion (double Tn, struct vectorDatos datos);
