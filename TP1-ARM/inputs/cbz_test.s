MOVZ X1, #0
MOVZ X2, #5
CBZ X1, salto_cbz
MOVZ X3, #0xDEAD
MOVZ X4, #0xBEEF

salto_cbz:
CBNZ X2, salto_cbnz
MOVZ X5, #0x1234  

salto_cbnz:
MOVZ X6, #0xAAAA

MOVZ X10, #0x0        
MOVZ X9,  #0x4000     
LSL  X9, X9, #16      
ADD  X10, X10, X9     

STUR X1, [X10, #0]
STUR X2, [X10, #8]
STUR X3, [X10, #16]
STUR X4, [X10, #24]
STUR X5, [X10, #32]
STUR X6, [X10, #40]

HLT #0
