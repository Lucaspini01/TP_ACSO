.text

MOVZ X0, #10
MOVZ X1, #10
MOVZ X2, #5

SUBS XZR, X0, X1
BEQ etiqueta_beq   

MOVZ X10, #0x1111
MOVZ X11, #0x1111

etiqueta_beq:

SUBS XZR, X0, X2
BNE etiqueta_bne     

MOVZ X12, #0x2222
MOVZ X13, #0x2222

etiqueta_bne:
SUBS XZR, X2, X0
BGT etiqueta_bgt     

MOVZ X14, #0x3333     

etiqueta_bgt:
BLT etiqueta_blt     


MOVZ X15, #0x4444

etiqueta_blt:
BGE etiqueta_bge     

MOVZ X16, #0x5555     

etiqueta_bge:
BLE etiqueta_ble    

MOVZ X17, #0x6666     

etiqueta_ble:
MOVZ X18, #0x7777     

HLT #0
