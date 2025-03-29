.text
    // X1 = 0x1000
    mov X1, 0x1000

    // X1 = X1 << 16 → 0x1000 << 16 = 0x10000000
    lsl X1, X1, 16

    // X10 = 0x1234
    mov X10, 0x1234

    // Guardamos X10 (0x0000000000001234) en memoria completa (8 bytes) en [X1 + 0x0]
    stur X10, [X1, 0x0]

    // Guardamos los 2 bytes bajos (0x1234) en [X1 + 0x6], pisa parte alta del stur
    sturh W10, [X1, 0x6]

    // Cargamos los 8 bytes desde X1 + 0x0 a X13
    ldur X13, [X1, 0x0]

    // Cargamos 8 bytes desde X1 + 0x4 a X14
    ldur X14, [X1, 0x4]

    // Cargamos 2 bytes desde X1 + 0x6 a W15 → deberíamos recuperar 0x1234
    ldurh W15, [X1, 0x6]

    HLT 0

/*
Salida esperada en rdump:

Current register/bus values :
-------------------------------------
Instruction Count : 9
PC                : 0x400024
Registers:
X0: 0x0
X1: 0x10000000
X2: 0x0
X3: 0x0
X4: 0x0
X5: 0x0
X6: 0x0
X7: 0x0
X8: 0x0
X9: 0x0
X10: 0x1234
X11: 0x0
X12: 0x0
X13: 0x1234000000001234     ; ← combinación de STUR y STURH
X14: 0x12340000              ; ← desde offset 0x4, contiene parte del stur + sturh
X15: 0x1234                  ; ← cargado con ldurh desde offset 0x6
X16: 0x0
X17: 0x0
X18: 0x0
X19: 0x0
X20: 0x0
X21: 0x0
X22: 0x0
X23: 0x0
X24: 0x0
X25: 0x0
X26: 0x0
X27: 0x0
X28: 0x0
X29: 0x0
X30: 0x0
X31: 0x0
FLAG_N: 0
FLAG_Z: 0
*/
