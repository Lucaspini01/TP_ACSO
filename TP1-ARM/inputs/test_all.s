        // =============================
        // Test de instrucciones ARMv8
        // =============================

        // MOVZ X1, #10
        movz    x1, #10

        // ADD (Immediate) X2 = X1 + 5
        add     x2, x1, #5

        // ADD (Extended Register) X3 = X1 + X2
        add     x3, x1, x2

        // ADDS (Immediate) X4 = X1 + 3 (set flags)
        adds    x4, x1, #3

        // ADDS (Extended Register) X5 = X1 + X2 (set flags)
        adds    x5, x1, x2

        // SUBS (Immediate) X6 = X1 - 2 (set flags)
        subs    x6, x1, #2

        // SUBS (Extended Register) X7 = X1 - X2 (set flags)
        subs    x7, x1, x2

        // CMP (Immediate) X1 - 4
        cmp     x1, #4

        // CMP (Extended Register) X1 - X2
        cmp     x1, x2

        // ANDS X8 = X1 & X2
        ands    x8, x1, x2

        // EOR X9 = X1 ^ X2
        eor     x9, x1, x2

        // ORR X10 = X1 | X2
        orr     x10, x1, x2

        // LSL X11 = X1 << 2
        lsl     x11, x1, #2

        // LSR X12 = X1 >> 1
        lsr     x12, x1, #1

        // STUR X1, [X2, #0x10]
        stur    x1, [x2, #0x10]

        // STURB X1, [X2, #0x14]
        sturb   w1, [x2, #0x14]

        // STURH X1, [X2, #0x18]
        sturh   w1, [x2, #0x18]

        // LDUR X13, [X2, #0x10]
        ldur    x13, [x2, #0x10]

        // LDURB X14, [X2, #0x14]
        ldurb   w14, [x2, #0x14]

        // LDURH X15, [X2, #0x18]
        ldurh   w15, [x2, #0x18]

        // B salto_directo
        b       salto_directo

        // Instrucción que debe saltearse
        movz    x16, #999

salto_directo:
        // Continuamos ejecución normalmente

        // Condicionales (se ejecutan según flags)
        cmp     x1, x1  // Igualdad

        beq     destino_beq
        bne     destino_bne
        bgt     destino_bgt
        blt     destino_blt
        bge     destino_bge
        ble     destino_ble

        // Marcadores de salto
destino_beq:
        movz    x17, #1
destino_bne:
        movz    x18, #2
destino_bgt:
        movz    x19, #3
destino_blt:
        movz    x20, #4
destino_bge:
        movz    x21, #5
destino_ble:
        movz    x22, #6

        // Fin de ejecución
        hlt     #0
