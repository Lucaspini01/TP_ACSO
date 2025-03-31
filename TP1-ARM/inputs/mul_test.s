.text

MOVZ    X1, #0x0003         
MOVZ    X2, #0x0020         
MUL     X3, X1, X2          
MOVZ    X4, #0x0000       
STUR    X3, [X4, #0]        
HLT     #0                  
