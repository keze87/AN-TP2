#include "greatest.h"
#include "CT29.h"

TEST testCargarDatos (void) {

	struct vectorDatos aux = cargarVectorDatos();

	GREATEST_ASSERT_EQ(aux.temperaturaInicial,		20 + 273);
	GREATEST_ASSERT_EQ(aux.densidad,				7850);
	GREATEST_ASSERT_EQ(aux.calorEspecifico,			480);
	GREATEST_ASSERT_IN_RANGE(aux.diametroExterno,	244.48 / 1000, 0.0001);
	GREATEST_ASSERT_IN_RANGE(aux.espesor,			013.84 / 1000, 0.0001);
	GREATEST_ASSERT_EQ(aux.longitudTubo,			12);
	GREATEST_ASSERT_EQ(aux.longitudHorno,			50);
	GREATEST_ASSERT_EQ(aux.bolsillos,				50);
	GREATEST_ASSERT_EQ(aux.cadencia,				35);
	GREATEST_ASSERT_EQ(aux.temperaturaUno,			500 + 273);
	GREATEST_ASSERT_EQ(aux.temperaturaDos,			500 + 273);
	GREATEST_ASSERT_EQ(aux.coeficienteConveccion,	20);
	GREATEST_ASSERT_IN_RANGE(aux.factorEmisividad,	0.85, 0.0001);

	GREATEST_ASSERT_IN_RANGE(aux.masa,			944.65123, 0.00003);
	GREATEST_ASSERT_IN_RANGE(aux.superficie,	17.4099026, 0.000001);
	GREATEST_ASSERT_IN_RANGE(aux.velocidad,		1 / 35, 0.000001);
	GREATEST_ASSERT_EQ(aux.tiempoEnElHorno,		1750);

	PASS();

}

double solucionAnalitica (int t, struct vectorDatos d /*datos*/) {

	return d.temperaturaUno + (d.temperaturaInicial - d.temperaturaUno) \
		* exp(-t * d.coeficienteConveccion * d.superficie / (d.masa * d.calorEspecifico));

}

TEST testEulerConveccion (void) {

	struct vectorDatos datos = cargarVectorDatos();

	TListaSimple lista = crearListaVI(datos.temperaturaInicial);

	euler(&lista, 1, datos);

	GREATEST_ASSERT(L_Vacia(lista) == FALSE);

	int retorno = L_Mover_Cte(&lista, L_Primero);

	while (retorno == TRUE) {

		struct elementoLista elem;

		L_Elem_Cte(lista, &elem);

		GREATEST_ASSERT_IN_RANGE(elem.T, solucionAnalitica(elem.t, datos), 0.1);

		retorno = L_Mover_Cte(&lista, L_Siguiente);

	}

	L_Vaciar(&lista);

	PASS();

}

// Main de pruebas unitarias:
GREATEST_MAIN_DEFS();

int correrTests () {

	GREATEST_MAIN_BEGIN();

	RUN_TEST(testCargarDatos);
	RUN_TEST(testEulerConveccion);

	GREATEST_MAIN_END();

}
