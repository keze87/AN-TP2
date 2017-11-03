
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
	double masa;
	double superficie;

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

};

struct vectorDatos cargarVectorDatos () {

	struct vectorDatos aux;

	aux.temperaturaInicial		= 20;
	aux.densidad				= 7850;
	aux.calorEspecifico			= 480;
	aux.diametroExterno			= 244.48 / 1000;
	aux.espesor					=  13.84 / 1000;
	aux.longitudTubo			= 12;
	aux.longitudHorno			= 50;
	aux.masa					= aux.densidad * PI * aux.diametroExterno * aux.espesor * \
										(1 - aux.espesor / aux.diametroExterno) * aux.longitudTubo;
	// 2πh(r1+r2) + 2π(r1)^2-(r2)^2
	aux.superficie				= 2 * PI * (aux.diametroExterno / 2 * (aux.longitudTubo + aux.diametroExterno / 2) \
											+ (aux.diametroExterno / 2 - aux.espesor) \
											* (aux.longitudTubo - (aux.diametroExterno / 2 - aux.espesor)));
	aux.bolsillos				= 50;
	aux.cadencia				= round( 35 -  10 / (10000 * (NUMERODEPADRON - 90000)));
	aux.temperaturaUno			= round(500 + 200 / (10000 * (NUMERODEPADRON - 90000))) + 273;
	aux.temperaturaDos			= round(500 + 200 / (10000 * (NUMERODEPADRON - 90000))) + 273;
	aux.coeficienteConveccion	= 20;
	aux.factorEmisividad		= 0.85;

	return aux;

}

int proceso () {

	return TRUE;

}

int main () {

	int aux = TRUE;

	aux = correrTests(); // Pruebas Unitarias

	if (aux == TRUE)
		return proceso();

	return aux;

}
