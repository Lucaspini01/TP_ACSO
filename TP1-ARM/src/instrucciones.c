#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include <stdint.h>
#include "shell.h"

#include "instrucciones.h"

#define XZR 31

void update_flags(int64_t result) {
    NEXT_STATE.FLAG_Z = (result == 0);
    NEXT_STATE.FLAG_N = (result < 0);
}

int64_t sign_extend(uint64_t value, int bits) {
    int64_t mask = 1LL << (bits - 1);
    return (value ^ mask) - mask;
}


void add_immediate(uint32_t instr){
    uint32_t rd = instr & 0x1F;              
    uint32_t rn = (instr >> 5) & 0x1F;      
    uint32_t imm12 = (instr >> 10) & 0xFFF;  
    uint32_t shift = (instr >> 22) & 0x1;    

    uint64_t imm = (shift == 1) ? (imm12 << 12) : imm12;

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + imm;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void add_shifted_register(uint32_t instr){
    uint32_t rd = instr & 0x1F;
    uint32_t rn = (instr >> 5) & 0x1F;
    uint32_t rm = (instr >> 16) & 0x1F;
    uint32_t shift = (instr >> 22) & 0x3;
    uint32_t amount = (instr >> 10) & 0x3F;

    uint64_t operand2 = CURRENT_STATE.REGS[rm];
    if (shift == 0) operand2 <<= amount;  // LSL
    else if (shift == 1) operand2 >>= amount;  // LSR

    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + operand2;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}

void adds_immediate(uint32_t instr) {
    uint32_t rd = instr & 0x1F;             
    uint32_t rn = (instr >> 5) & 0x1F;      
    uint32_t imm12 = (instr >> 10) & 0xFFF;
    uint32_t shift = (instr >> 22) & 0x3;   

    uint64_t imm = (shift == 1) ? (imm12 << 12) : imm12;

    int64_t result = CURRENT_STATE.REGS[rn] + imm;

    update_flags(result);

    if (rd != XZR) NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void adds_extended_register(uint32_t instr) {
    uint32_t rd = instr & 0x1F;        
    uint32_t rn = (instr >> 5) & 0x1F; 
    uint32_t rm = (instr >> 16) & 0x1F;

    int64_t result = CURRENT_STATE.REGS[rn] + CURRENT_STATE.REGS[rm];

    NEXT_STATE.REGS[rd] = result;

    update_flags(result);

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void subs_immediate(uint32_t instr){
    uint32_t rd = instr & 0x1F;             
    uint32_t rn = (instr >> 5) & 0x1F;      
    uint32_t imm12 = (instr >> 10) & 0xFFF; 
    uint32_t shift = (instr >> 22) & 0x3;   

    uint64_t imm = (shift == 1) ? (imm12 << 12) : imm12;

    int64_t result = CURRENT_STATE.REGS[rn] - imm;

    update_flags(result);

    if (rd != XZR) NEXT_STATE.REGS[rd] = result;

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void subs_extended_register(uint32_t instr){
    uint32_t rd = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        uint32_t rm = (instr >> 16) & 0x1F;

        int64_t result = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];

        if (rd != XZR) NEXT_STATE.REGS[rd] = result;

        update_flags(result);

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void mul(uint32_t instr){
    uint32_t rd = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        uint32_t rm = (instr >> 16) & 0x1F;

        uint64_t result = CURRENT_STATE.REGS[rn] * CURRENT_STATE.REGS[rm];

        if (rd != XZR) {
            NEXT_STATE.REGS[rd] = result;
        }

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void ands(uint32_t instr){
    uint32_t rd = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        uint32_t rm = (instr >> 16) & 0x1F;


        int64_t result = CURRENT_STATE.REGS[rn] & CURRENT_STATE.REGS[rm];
        NEXT_STATE.REGS[rd] = result;
        update_flags(result);

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void eor(uint32_t instr){
    uint32_t rd = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        uint32_t rm = (instr >> 16) & 0x1F;

        
        int64_t result = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];
        NEXT_STATE.REGS[rd] = result;

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void orr(uint32_t instr){
    uint32_t rd = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        uint32_t rm = (instr >> 16) & 0x1F;
        

        int64_t result = CURRENT_STATE.REGS[rn] | CURRENT_STATE.REGS[rm];
        NEXT_STATE.REGS[rd] = result;

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void movz(uint32_t instr){
    uint32_t rd = instr & 0x1F;                 
        uint64_t imm16 = (instr >> 5) & 0xFFFF;     
        uint32_t hw = (instr >> 21) & 0x3;          
    
        if (hw == 0) {
            NEXT_STATE.REGS[rd] = imm16;
        }
    
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void stur(uint32_t instr){
    uint32_t rt = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        int32_t imm9 = (instr >> 12) & 0x1FF;
        imm9 = (imm9 << 23) >> 23; 
    
        uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
        mem_write_32(addr, (uint32_t)(CURRENT_STATE.REGS[rt] & 0xFFFFFFFF));
        mem_write_32(addr + 4, (uint32_t)(CURRENT_STATE.REGS[rt] >> 32));
    
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void sturb(uint32_t instr){
    uint32_t rt = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        int32_t imm9 = (instr >> 12) & 0x1FF;
        imm9 = (imm9 << 23) >> 23;
    
        uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
        uint8_t byte_val = CURRENT_STATE.REGS[rt] & 0xFF;
        mem_write_32(addr, byte_val);  
    
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void sturh(uint32_t instr){
    uint32_t rt = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        int32_t imm9 = (instr >> 12) & 0x1FF;
        imm9 = (imm9 << 23) >> 23;
    
        uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
        uint16_t half = CURRENT_STATE.REGS[rt] & 0xFFFF;
    
        uint32_t value = mem_read_32(addr);
        value = (value & 0xFFFF0000) | half;
        mem_write_32(addr, value);
    
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void ldur(uint32_t instr){
    uint32_t rt = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        int32_t imm9 = (instr >> 12) & 0x1FF;
        imm9 = (imm9 << 23) >> 23;  
    
        uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
    
        uint64_t low = mem_read_32(addr);
        uint64_t high = mem_read_32(addr + 4);
        NEXT_STATE.REGS[rt] = (high << 32) | low;
    
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void ldurb(uint32_t instr){
    uint32_t rt = instr & 0x1F;
    uint32_t rn = (instr >> 5) & 0x1F;
    int32_t imm9 = (instr >> 12) & 0x1FF;
    imm9 = (imm9 << 23) >> 23;  

    uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
    uint32_t word = mem_read_32(addr & ~0x3);  
    uint8_t byte = (word >> ((addr & 0x3) * 8)) & 0xFF;

    NEXT_STATE.REGS[rt] = (uint64_t)byte;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void ldurh(uint32_t instr){
    uint32_t rt = instr & 0x1F;
    uint32_t rn = (instr >> 5) & 0x1F;
    int32_t imm9 = (instr >> 12) & 0x1FF;
    imm9 = (imm9 << 23) >> 23;

    uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
    uint32_t word = mem_read_32(addr & ~0x3);
    uint16_t half = (word >> ((addr & 0x3) * 8)) & 0xFFFF;

    NEXT_STATE.REGS[rt] = (uint64_t)half;
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void b(uint32_t instr){
     int32_t imm26 = instr & 0x03FFFFFF;
     int64_t offset = sign_extend(imm26 << 2, 28); 

     NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}

void br(uint32_t instr){
    uint32_t rn = (instr >> 5) & 0x1F; 
    printf("Ejecutando BR a: 0x%08lx\n", CURRENT_STATE.REGS[rn]);
    printf("Instrucción en destino: 0x%08x\n", mem_read_32(CURRENT_STATE.REGS[rn]));
    NEXT_STATE.PC = CURRENT_STATE.REGS[rn];
}


void b_cond(uint32_t instr){
    uint32_t cond = instr & 0xF; 

        bool should_branch = false;

        switch (cond) {
            case 0b0000: // BEQ
                should_branch = (CURRENT_STATE.FLAG_Z == 1);
                break;
            case 0b0001: // BNE
                should_branch = (CURRENT_STATE.FLAG_Z == 0);
                break;
            case 0b1010: // BGE
                should_branch = (CURRENT_STATE.FLAG_N == 0);
                break;
            case 0b1011: // BLT
                should_branch = (CURRENT_STATE.FLAG_N == 1);
                break;
            case 0b1100: // BGT
                should_branch = (CURRENT_STATE.FLAG_N == 0);
                break;
            case 0b1101: // BLE
                should_branch = (CURRENT_STATE.FLAG_Z == 1 || CURRENT_STATE.FLAG_N == 1);
                break;
            default:
                break;
        }

        if (should_branch) {
            int32_t imm19 = (instr >> 5) & 0x7FFFF; 
            int64_t offset = sign_extend(imm19, 19) << 2;
    
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
        } else {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        }
}

void cbz(uint32_t instr){
    uint32_t rt = instr & 0x1F;
    int32_t imm19 = (instr >> 5) & 0x7FFFF;
    int64_t offset = sign_extend(imm19 << 2, 21);

    if (CURRENT_STATE.REGS[rt] == 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void cbnz(uint32_t instr){
    uint32_t rt = instr & 0x1F;
    int32_t imm19 = (instr >> 5) & 0x7FFFF;
    int64_t offset = sign_extend(imm19 << 2, 21);

    if (CURRENT_STATE.REGS[rt] != 0) {
        NEXT_STATE.PC = CURRENT_STATE.PC + offset;
    } else {
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void lsl_lsr(uint32_t instr){
    uint32_t rd = instr & 0x1F;
    uint32_t rn = (instr >> 5) & 0x1F;
    uint32_t immr = (instr >> 16) & 0x3F;
    uint32_t imms = (instr >> 10) & 0x3F;
    uint32_t N = (instr >> 22) & 0x1;
    uint32_t sf = (instr >> 31) & 0x1;
    
    

    if (sf == 1 && N == 1) {
        enum { LSL, LSR, OTHER } op_type;

        if (imms != 63) {
            op_type = LSL;
        } else if (imms == 63) {
            op_type = LSR;
        } else {
            op_type = OTHER;
        }

        uint64_t value = CURRENT_STATE.REGS[rn];
        uint64_t result = 0;


        switch (op_type) {
            case LSL:
                result = value << (64 - immr);
                break;
            case LSR:
                result = value >> immr;
                break;
            case OTHER:
                break; 
        }

        if (op_type != OTHER && rd != XZR) {
            NEXT_STATE.REGS[rd] = result;
        }

        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
    }
}

void hlt(uint32_t instr){
    RUN_BIT = 0;
    printf("Instrucción HALT detected\n");

    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}
