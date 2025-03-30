        .text
        .global _start

_start:
        MOVZ    X1, #0x0003         // X1 = 3
        MOVZ    X2, #0x0020         // X2 = 32
        MUL     X3, X1, X2          // X3 = X1 * X2 = 96
        MOVZ    X4, #0x0000         // X4 = 0 (dirección base)
        STUR    X3, [X4, #0]        // Guarda X3 en mem[0]
        HLT     #0                  // Finaliza el programa
