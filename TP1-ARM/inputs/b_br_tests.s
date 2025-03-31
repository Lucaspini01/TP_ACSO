.text
    mov X0, 42
    b etiqueta_b
    mov X0, 999

etiqueta_b:
    mov X1, 0x4000
    lsl X1, X1, 12
    add X1, X1, 0x28
    br X1
    mov X1, 999

destino:
    mov X2, 123
    hlt 0

