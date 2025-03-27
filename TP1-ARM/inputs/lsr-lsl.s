// =============================
// Test LSL y LSR usando alias UBFM
// =============================

        // Cargar valor base
        movz    x1, #0x1         // x1 = 0x0000000000000001

        // LSL x2 = x1 << 4 (0x1 << 4 = 0x10)
        lsl     x2, x1, #4       // Esperado: 0x10

        // LSL x3 = x1 << 8 (0x1 << 8 = 0x100)
        lsl     x3, x1, #8       // Esperado: 0x100

        // Cargar valor más alto
        movz    x4, #0x8000      // x4 = 0x0000000000008000

        // LSR x5 = x4 >> 4 (0x8000 >> 4 = 0x800)
        lsr     x5, x4, #4       // Esperado: 0x800

        // LSR x6 = x4 >> 8 (0x8000 >> 8 = 0x80)
        lsr     x6, x4, #8       // Esperado: 0x80

        // HLT para detener ejecución
        hlt     #0
