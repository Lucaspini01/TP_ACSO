.text
movz    x1, #0x1        
LSL     x2, x1, #4       
LSL     x3, x1, #8       
movz    x4, #0x8000      
LSR     x5, x4, #4       
LSR     x6, x4, #8       
hlt     #0
