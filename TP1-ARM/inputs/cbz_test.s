// cbz_test.s

// Inicializar X1 = 0 y X2 = 5
MOVZ X1, #0
MOVZ X2, #5

// CBZ: salta si X1 == 0 → debería saltar las 2 siguientes
CBZ X1, salto_cbz

// Si CBZ no salta, estas instrucciones pondrían valores en X3 y X4 (no deberían ejecutarse)
MOVZ X3, #0xDEAD
MOVZ X4, #0xBEEF

salto_cbz:
// CBNZ: salta si X2 != 0 → debería saltar la siguiente
CBNZ X2, salto_cbnz

MOVZ X5, #0x1234  // Esta no debería ejecutarse si el salto se hizo

salto_cbnz:
// Valor que siempre debería escribirse
MOVZ X6, #0xAAAA

// Inicializar X10 = 0x400000 sin usar MOVK
MOVZ X10, #0x0        // X10 = 0
MOVZ X9,  #0x4000     // X9 = 0x4000
LSL  X9, X9, #16      // X9 = 0x400000
ADD  X10, X10, X9     // X10 = 0x400000

// Guardar registros
STUR X1, [X10, #0]
STUR X2, [X10, #8]
STUR X3, [X10, #16]
STUR X4, [X10, #24]
STUR X5, [X10, #32]
STUR X6, [X10, #40]

// Terminar
HLT #0
