
#include "tests.h" // Pruebas Unitarias

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

struct vectorDatos cargarVectorDatos () {

	struct vectorDatos aux;

	aux.temperaturaInicial		= 20;
	aux.densidad				= 7850;
	aux.calorEspecifico			= 480;
	aux.diametroExterno			= 244.48 / 1000;
	aux.espesor					=  13.84 / 1000;
	aux.longitudTubo			= 12;
	aux.longitudHorno			= 50;
	aux.bolsillos				= 50;
	aux.cadencia				= round( 35 -  10 / (10000 * (NUMERODEPADRON - 90000)));
	aux.temperaturaUno			= round(500 + 200 / (10000 * (NUMERODEPADRON - 90000))) + 273;
	aux.temperaturaDos			= round(500 + 200 / (10000 * (NUMERODEPADRON - 90000))) + 273;
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

double euler (double Tn, int t, int h, struct vectorDatos datos) {

	double Tn1 = Tn + h * fConveccion(Tn, datos);

	printf("%d -> %F\n", t, Tn1);

	if (t < datos.tiempoEnElHorno)
		return euler(Tn1, t + h, h, datos);

	return Tn1;

}

int resolverConveccion () {

	struct vectorDatos datos = cargarVectorDatos();

	int h = buscarEstabilidad(datos);

	euler(20, 1, h, datos);

	return TRUE;

}

int proceso () {

	imprimirEnunciado(1);
	resolverConveccion();

	//imprimirEnunciado(2);


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

