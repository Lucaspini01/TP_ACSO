// test_beq_cmp.s

.text

    // Instrucción 1: cmp X11, X11
    // Compara X11 con X11 → 0 - 0 = 0 → FLAG_Z = 1, FLAG_N = 0
    cmp X11, X11

    // Instrucción 2: beq foo
    // Como FLAG_Z == 1, salta a 'foo' (saltea la siguiente instrucción)
    beq foo

    // Instrucción 3 (salteada): suma 10 + X0 → debería escribir en X2 (NO SE EJECUTA)
    adds X2, X0, #10

bar:
    // Instrucción 4: llega por salto → halt del programa
    hlt 0

foo:
    // Instrucción 5: cmp X11, X11 → otra vez resultado 0 → FLAG_Z = 1
    cmp X11, X11

    // Instrucción 6: beq bar → salta de nuevo a 'bar' (otra vez saltea la suma)
    beq bar

    // Instrucción 7 (NO SE EJECUTA): suma 10 + X0 → debería escribir en X3
    adds X3, X0, #10

    // Instrucción 8 (NO SE EJECUTA): halt alternativo
    hlt 0


/*
╔════════════════════════════════════════════════════╗
║      Salida esperada de `rdump` al final          ║
╚════════════════════════════════════════════════════╝

Current register/bus values :
-------------------------------------
Instruction Count : 5
PC                : 0x400014
Registers:
X0: 0x0
X1: 0x0
X2: 0x0       ← no fue escrita (salteada por primer beq)
X3: 0x0       ← no fue escrita (salteada por segundo beq)
X4: 0x0
X5: 0x0
...
X10: 0x0
X11: 0x0      ← usado en cmp
...
X30: 0x0
X31: 0x0
FLAG_N: 0
FLAG_Z: 1     ← resultado de cmp fue 0
*/
