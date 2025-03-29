.text
    // X0 = 42
    mov X0, 42

    // Salto incondicional al label 'etiqueta_b'
    b etiqueta_b

    // Esta instrucción NO debe ejecutarse si el salto B funciona bien
    mov X0, 999

etiqueta_b:
    // X1 = 0x4000
    mov X1, 0x4000

    // X1 = X1 << 12 → 0x4000 << 12 = 0x4000000
    lsl X1, X1, 12

    // X1 = X1 + 0x28 → ahora apunta a la dirección del label 'destino'
    add X1, X1, 0x28

    // Saltamos a la dirección contenida en X1 → br destino
    br X1

    // Esto tampoco debería ejecutarse si el salto BR funciona bien
    mov X1, 999

destino:
    // Confirmación de que llegamos correctamente al destino
    mov X2, 123

    hlt 0

/*
Instruction Count: 7
PC                : 0x40002C
Registers:
X0: 0x2A         ; 42 en hexa → ejecutado antes del primer salto
X1: 0x400028     ; contiene la dirección de la etiqueta 'destino'
X2: 0x7B         ; 123 en hexa → indica que BR funcionó bien
X3: 0x0
...
X31: 0x0
FLAG_N: 0
FLAG_Z: 0

*/