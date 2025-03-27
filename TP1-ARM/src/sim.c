#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <stdint.h>
#include "shell.h"

#define XZR 31

void update_flags(int64_t result) {
    NEXT_STATE.FLAG_Z = (result == 0);
    NEXT_STATE.FLAG_N = (result < 0);
}


void process_instruction() {

    uint32_t instr = mem_read_32(CURRENT_STATE.PC);
    printf("Instrucción: 0x%08x\n", instr);

    uint32_t opcode = (instr >> 21) & 0x7FF;
    printf("Opcode: 0x%03x\n", opcode);

    switch (opcode) {

        // ----------- ADD (Immediate) - sin flags -----------
        case 0x588:
        case 0x488: {
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t imm12 = (instr >> 10) & 0xFFF;
            uint32_t shift = (instr >> 22) & 0x3;

            if (shift == 0 || shift == 1) {
                uint64_t imm = (shift == 1) ? (imm12 << 12) : imm12;
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + imm;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            } else {
                printf("ADD (imm): shift inválido (shift=%u)\n", shift);
                RUN_BIT = 0;
            }
            break;
        }

        // ----------- ADDS (Register) - con flags -----------
        case 0x558:
        {
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;

            int64_t result = CURRENT_STATE.REGS[rn] + CURRENT_STATE.REGS[rm];
            NEXT_STATE.REGS[rd] = result;
            update_flags(result);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

        // ----------- ADDS (Immediate) - con flags -----------
        case 0x798:
        case 0x718:
        case 0x58A: {
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t imm12 = (instr >> 10) & 0xFFF;
            uint32_t shift = (instr >> 22) & 0x3;

            if (shift == 0 || shift == 1) {
                uint64_t imm = (shift == 1) ? (imm12 << 12) : imm12;
                int64_t result = CURRENT_STATE.REGS[rn] + imm;
                NEXT_STATE.REGS[rd] = result;
                update_flags(result);
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            } else {
                printf("ADDS (imm): shift inválido (shift=%u)\n", shift);
                RUN_BIT = 0;
            }
            break;
        }

        // ----------- SUB (Register) - sin flags -----------
        case 0x458:
        case 0x658: {
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;

            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

        // ----------- SUBS (Immediate) - con flags -----------
        case 0x788: {
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t imm12 = (instr >> 10) & 0xFFF;
            uint32_t shift = (instr >> 22) & 0x3;

            uint64_t imm = (shift == 1) ? (imm12 << 12) : imm12;
            int64_t result = CURRENT_STATE.REGS[rn] - imm;
            update_flags(result);
            if (rd != XZR) NEXT_STATE.REGS[rd] = result;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            return;
        }

        // ----------- SUBS (Register) - con flags -----------
        case 0x758: {
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;

            int64_t result = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];
            update_flags(result);
            if (rd != XZR) NEXT_STATE.REGS[rd] = result;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            return;
        }

        // ----------- ANDS -----------
        case 0x750: { // ANDS
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;


            int64_t result = CURRENT_STATE.REGS[rn] & CURRENT_STATE.REGS[rm];
            NEXT_STATE.REGS[rd] = result;
            update_flags(result);

            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x650: { // EOR
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;

            
            int64_t result = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];
            NEXT_STATE.REGS[rd] = result;

            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        
        case 0b10101010000: { // ORR
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;
            

            int64_t result = CURRENT_STATE.REGS[rn] | CURRENT_STATE.REGS[rm];
            NEXT_STATE.REGS[rd] = result;

            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

        case 0x358: { // UBFM
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t immr = (instr >> 16) & 0x3F;
            uint32_t imms = (instr >> 10) & 0x3F;
        
            // Alias: LSR Xd, Xn, #shift
            if (imms == 63) {
                uint64_t value = (uint64_t)CURRENT_STATE.REGS[rn];
                NEXT_STATE.REGS[rd] = value >> immr;
            }
        
            // Alias: LSL Xd, Xn, #shift
            else if (((immr + 1) & 0x3F) == imms) {
                uint64_t value = (uint64_t)CURRENT_STATE.REGS[rn];
                uint32_t shift = 63 - imms;
                NEXT_STATE.REGS[rd] = value << shift;
            }
        
            // Otra forma de UBFM no implementada
            else {
                printf("UBFM no reconocido como alias de LSL/LSR. immr=%u, imms=%u\n", immr, imms);
                RUN_BIT = 0;
            }
        
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

        
        case 0x694: { // MOVZ (hw = 0)
            uint32_t rd = instr & 0x1F;                 // bits [4:0]: destino
            uint64_t imm16 = (instr >> 5) & 0xFFFF;     // bits [20:5]: inmediato de 16 bits
            uint32_t hw = (instr >> 21) & 0x3;          // bits [22:21]: shift selector (hw)
        
            if (hw == 0) {
                NEXT_STATE.REGS[rd] = imm16;
            }
        
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        
        case 0x7C0: { // STUR
            uint32_t rt = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            int32_t imm9 = (instr >> 12) & 0x1FF;
            imm9 = (imm9 << 23) >> 23;  // Sign-extend 9 bits
        
            uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
            mem_write_32(addr, (uint32_t)(CURRENT_STATE.REGS[rt] & 0xFFFFFFFF));
            mem_write_32(addr + 4, (uint32_t)(CURRENT_STATE.REGS[rt] >> 32));
        
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        
        case 0x1C0: { // STURB
            uint32_t rt = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            int32_t imm9 = (instr >> 12) & 0x1FF;
            imm9 = (imm9 << 23) >> 23;
        
            uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
            uint8_t byte_val = CURRENT_STATE.REGS[rt] & 0xFF;
            mem_write_32(addr, byte_val);  // Solo escribimos 1 byte en realidad
        
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        
        case 0x5C0: { // STURH
            uint32_t rt = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            int32_t imm9 = (instr >> 12) & 0x1FF;
            imm9 = (imm9 << 23) >> 23;
        
            uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
            uint16_t half = CURRENT_STATE.REGS[rt] & 0xFFFF;
        
            // Escribimos los 2 bytes en dos write separados (o extendemos a 4)
            uint32_t value = mem_read_32(addr);
            value = (value & 0xFFFF0000) | half;
            mem_write_32(addr, value);
        
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        
        case 0x7C2: { // LDUR
            uint32_t rt = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            int32_t imm9 = (instr >> 12) & 0x1FF;
            imm9 = (imm9 << 23) >> 23;  // sign-extend de 9 bits
        
            uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
        
            uint64_t low = mem_read_32(addr);
            uint64_t high = mem_read_32(addr + 4);
            NEXT_STATE.REGS[rt] = (high << 32) | low;
        
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        
        case 0x1C2: { // LDURB
            uint32_t rt = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            int32_t imm9 = (instr >> 12) & 0x1FF;
            imm9 = (imm9 << 23) >> 23;  // sign-extend 9 bits
        
            uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
            uint32_t word = mem_read_32(addr & ~0x3);  // leer la palabra alineada
            uint8_t byte = (word >> ((addr & 0x3) * 8)) & 0xFF;
        
            NEXT_STATE.REGS[rt] = (uint64_t)byte;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        
        case 0x5C2: { // LDURH
            uint32_t rt = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            int32_t imm9 = (instr >> 12) & 0x1FF;
            imm9 = (imm9 << 23) >> 23;
        
            uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
            uint32_t word = mem_read_32(addr & ~0x3);
            uint16_t half = (word >> ((addr & 0x3) * 8)) & 0xFFFF;
        
            NEXT_STATE.REGS[rt] = (uint64_t)half;
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

        // ----------- B (Branch incondicional) -----------
        case 0x0A0: {
            int32_t imm26 = instr & 0x03FFFFFF;
            if (imm26 & (1 << 25)) imm26 |= 0xFC000000; // Sign extend
            int32_t offset = imm26 << 2;
            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
            break;
        }

        // ----------- B.cond (Branch condicional) -----------
        case 0x2A0: { // opcode para B.cond
            int32_t imm19 = (instr >> 5) & 0x7FFFF;  // bits 23-5
            uint32_t cond = instr & 0xF;             // bits 3-0

            if (imm19 & (1 << 18)) imm19 |= 0xFFF80000;  // Sign extend
            int32_t offset = imm19 << 2;

            int should_branch = 0;
            switch (cond) {
                case 0x0:  // EQ (Z == 1)
                    should_branch = NEXT_STATE.FLAG_Z;
                    break;
                case 0x1:  // NE (Z == 0)
                    should_branch = !NEXT_STATE.FLAG_Z;
                    break;
                case 0xC:  // LT (N != V), como V=0, equivale a N==1
                    should_branch = NEXT_STATE.FLAG_N;
                    break;
                case 0xD:  // GE (N == V), como V=0, equivale a N==0
                    should_branch = !NEXT_STATE.FLAG_N;
                    break;
                case 0xE:  // AL (siempre)
                    should_branch = 1;
                    break;
                default:
                    printf("Condición no soportada: %u\n", cond);
                    RUN_BIT = 0;
                    return;
            }

            if (should_branch)
                NEXT_STATE.PC = CURRENT_STATE.PC + offset;
            else
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;

            break;
        }
    
        case 0x6a2: {   // HALT: Detener la simulación
            RUN_BIT = 0;
            printf("Instrucción HALT detected\n");
            break;
        }
        default: {
            // Si la instrucción no es reconocida, detener la simulación
            RUN_BIT = 0;
            printf("Instrucción no reconocida: 0x%08x\n", instr);
            break;
        }
    }

}