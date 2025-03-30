#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include <stdint.h>
#include "shell.h"

void process_instruction() {

    uint32_t instr = mem_read_32(CURRENT_STATE.PC);
    printf("Instrucción: 0x%08x\n", instr);

    uint32_t opcode11 = (instr >> 21) & 0x7FF;
    uint32_t opcode10 = (instr >> 22) & 0x3FF;
    uint32_t opcode9  = (instr >> 23) & 0x1FF;
    uint32_t opcode6  = (instr >> 26) & 0x3F;

    printf("Opcode11: 0x%03x\n", opcode11);



}


    switch (opcode11) {

        // ----------- ADD (Immediate) - sin flags -----------
        case 0b10010001100:
        case 0b10010001010:
        {
        }
    // ----------- ADD (Shifted Register) (opcode: 0b10001011000) -----------
        case 0b10010001000:
        {
        }

        // ----------- ADDS (Immediate) - con flags -----------
        case 0b10110001010: // Opcode for ADDS (Immediate) with flags
        case 0b10110001000: // Alternate opcode for ADDS (Immediate) with flags
        {
           
        }

        // ----------- ADDS (Extended Register) - con flags -----------
        case 0b10101011000:
        {
            
        }
   

        // ----------- SUBS (Immediate) - con flags -----------
        case 0b11110001000:
        case 0b11110001010: 
        {
            
        }

        // ----------- SUBS (Extended Register) - con flags -----------
        case 0b11101011000:
         {
            
        }

        // ----------- MUL (Multiply: Rd = Rn * Rm) -----------
        case 0b10011011000: {  // opcode11 == 0x4D8
            
        }


        // ----------- ANDS -----------
        case 0b11101010000: {
            
        }

         // ----------- EOR -----------

        case 0b11001010000: { 
            
        }
        
         // ----------- ORR -----------

        case 0b10101010000: 
        {
            
        }

         // ----------- MOVZ (hw = 0) -----------

        case 0b11010010100: 
        { 
            
        }

    
        // ----------- STUR -----------
        
        case 0b11111000000: {
            
        }
        
        case 0b111000000: { // STURB
            
        }
        case 0b01111000000: { // STURH (opcode11 == 0x3C0)
            printf(">> Entrando al case STURH (0x3C0)\n");
            
        }
        
        case 0b11111000010: { // LDUR
            
        }
        
        case 0b111000010: { // LDURB
            
        }
        
        case 0b01111000010: { // LDURH (opcode11 == 0x3C2)
            printf(">> Entrando al case LDURH (0x3C2)\n");
            
        }               

       // ----------- B (Branch incondicional) -----------
        case 0b00010100000: 
        {
           
        }

        case 0b11010110000: {  // BR
            
        }
        
        // ----------- B.cond (Branch condicional) -----------
        case 0b1010100000:
               
        {
            
        }

        // ----------- CBZ y CBNZ (opcode6 == 0x0D) -----------
        // ----------- CBZ ----------- (opcode11 == 0b10110100000)
        case 0b10110100000: {
            
        }

        // ----------- CBNZ ----------- (opcode11 == 0b10110100001)
        case 0b10110100001: {
            
        }

        // -----------  LSL - LSR ----------------------------
        //EL ultimo bit no es del opcode, es del immr por eso los dos casos.
        case 0b11010011011:
        case 0b11010011010: 
        {
            
        }
    
        case 0b11010100010: {   // HALT: Detener la simulación
            
        }
    
        default :
        //: {
            printf("Instrucción no implementada: 0x%08x\n", instr);
            RUN_BIT = 0;
            return;
        }

            

    }

    

