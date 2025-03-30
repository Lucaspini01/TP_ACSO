#include <stdio.h>
#include <stdint.h>
#include "shell.h"
#include "sim.h"

InstructionEntry instruction_set[] = {
    {0xB1     << 24, 0xFF000000, adds_immediate},        
    {0xAB0    << 20, 0xFFF00000, adds_extended_register},
    {0xF1     << 24, 0xFF000000, subs_immediate},         
    {0xEB0    << 20, 0xFFF00000, subs_extended_register}, 
    {0xD44    << 20, 0xFFF00000, hlt},                             
    {0xEA     << 24, 0xFF000000, ands}, 
    {0xCA     << 24, 0xFF000000, eor},  
    {0xAA     << 24, 0xFF000000, orr},  
    {0x03     << 26, 0xFC000000, b},              
    {0x3587C0 << 10, 0xFFFFFC00, br},   
    {0x54     << 24, 0xFF000000, b_cond},  
    {0xD3     << 24, 0xFF000000, lsl_lsr},
    {0xF80    << 20, 0xFFF00000, stur},
    {0x380    << 20, 0xFFF00000, sturb},
    {0x780    << 20, 0xFFF00000, sturh},
    {0xF84    << 20, 0xFFF00000, ldur},
    {0x784    << 20, 0xFFF00000, ldurh},
    {0x384    << 20, 0xFFF00000, ldurb},
    {0xD2     << 24, 0xFF000000, movz},
    {0x91     << 24, 0xFF000000, add_immediate},        
    {0x8B0    << 20, 0xFFF00000, add_shifted_register},
    {0x9B0    << 20, 0xFFF00000, mul},
    {0xB4     << 24, 0xFF000000, cbz},
    {0xB5     << 24, 0xFF000000, cbnz},                    
    
    {0, 0, NULL} 
};

void process_instruction() {
    uint32_t instr = mem_read_32(CURRENT_STATE.PC);
    for (int i = 0; instruction_set[i].execute != NULL; i++) {
        if ((instr & instruction_set[i].mask) == instruction_set[i].opcode) {
            instruction_set[i].execute(instr);
            return;
        }
    }

    printf("Instrucción no implementada: 0x%08x\n", instr);
    RUN_BIT = 0;
}
