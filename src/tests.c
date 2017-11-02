#include "greatest.h"
#include "CT29.h"

TEST testAuxiliares (void) {

	PASS();

}

// Main de pruebas unitarias:
GREATEST_MAIN_DEFS();

int correrTests () {

	GREATEST_MAIN_BEGIN();

	RUN_TEST(testAuxiliares);

	GREATEST_MAIN_END();

}
