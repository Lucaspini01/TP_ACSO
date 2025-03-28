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

    uint32_t opcode11 = (instr >> 21) & 0x7FF;
    uint32_t opcode10 = (instr >> 22) & 0x3FF;
    uint32_t opcode9  = (instr >> 23) & 0x1FF;
    uint32_t opcode6  = (instr >> 26) & 0x3F;

    printf("Opcode11: 0x%03x\n", opcode11);


    switch (opcode11) {

        // ----------- ADD (Immediate) - sin flags -----------
        case 0b10010001100:
        case 0b10010001010:
        {
            uint32_t rd = instr & 0x1F;             // bits [4:0]
        uint32_t rn = (instr >> 5) & 0x1F;      // bits [9:5]
        uint32_t rm = (instr >> 16) & 0x1F;     // bits [20:16]
        uint32_t imm6 = (instr >> 10) & 0x3F;   // bits [15:10]
        uint32_t shift_type = (instr >> 22) & 0x3; // bits [23:22]

        uint64_t operand2 = CURRENT_STATE.REGS[rm];

        // Aplicar el shift correspondiente
        switch (shift_type) {
            case 0b00: // LSL
                operand2 <<= imm6;
                break;
            case 0b01: // LSR
                operand2 >>= imm6;
                break;
            case 0b10: // ASR
                operand2 = ((int64_t)operand2) >> imm6;
                break;
            default:
                printf("Shift inválido para ADD (shift=%u)\n", shift_type);
                RUN_BIT = 0;
                return;
        }

        NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rn] + operand2;
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
        break;
    }
        // ----------- ADDS (Immediate) - con flags -----------
        case 0b10110001010: // Opcode for ADDS (Immediate) with flags
        case 0b10110001000: // Alternate opcode for ADDS (Immediate) with flags
        {
            uint32_t rd = instr & 0x1F;             // Extract destination register (bits [4:0])
            uint32_t rn = (instr >> 5) & 0x1F;      // Extract first operand register (bits [9:5])
            uint32_t imm12 = (instr >> 10) & 0xFFF; // Extract 12-bit immediate value (bits [21:10])
            uint32_t shift = (instr >> 22) & 0x3;   // Extract shift type (bits [23:22])

            // Apply shift to the immediate value if required
            uint64_t imm = (shift == 1) ? (imm12 << 12) : imm12;

            // Perform addition of the first operand and the immediate value
            int64_t result = CURRENT_STATE.REGS[rn] + imm;

            // Update the processor flags (Zero and Negative) based on the result
            update_flags(result);

            // Write the result to the destination register if it's not XZR (zero register)
            if (rd != XZR) NEXT_STATE.REGS[rd] = result;

            // Increment the program counter to point to the next instruction
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            return;
        }

        // ----------- ADDS (Extended Register) - con flags -----------
        case 0b10101011001:
        {
            uint32_t rd = instr & 0x1F;        // Destino Xd (bits 4-0)
            uint32_t rn = (instr >> 5) & 0x1F; // Primer operando Xn (bits 9-5)
            uint32_t rm = (instr >> 16) & 0x1F;// Segundo operando Xm (bits 20-16)

            int64_t result = CURRENT_STATE.REGS[rn] + CURRENT_STATE.REGS[rm];

            // Guardar el resultado en el registro destino
            NEXT_STATE.REGS[rd] = result;

            // Actualizar flags
            update_flags(result);

            // Incrementar el PC
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
   

        // ----------- SUBS (Immediate) - con flags -----------
        case 0b10110001000: 
        {
            uint32_t rd = instr & 0x1F;             // bits [4:0]
            uint32_t rn = (instr >> 5) & 0x1F;      // bits [9:5]
            uint32_t imm12 = (instr >> 10) & 0xFFF; // bits [21:10]
            uint32_t shift = (instr >> 22) & 0x3;   // bits [23:22]
    
            // Aplica shift al valor inmediato si es necesario
            uint64_t imm = (shift == 1) ? (imm12 << 12) : imm12;
    
            // Realiza la resta: operand1 - inmediato
            int64_t result = CURRENT_STATE.REGS[rn] - imm;
    
            // Actualiza los flags N y Z
            update_flags(result);
    
            // Guarda el resultado en rd si no es XZR
            if (rd != XZR) NEXT_STATE.REGS[rd] = result;
    
            // Avanza el program counter
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            return;
        }

        // ----------- SUBS (Extended Register) - con flags -----------
        case 0b11101011001:
         { // SUBS
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;

            int64_t result = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];

            // Update flags
            update_flags(result);

            // Si no es CMP, guardar resultado
            if (rd != XZR) NEXT_STATE.REGS[rd] = result;

            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            return;
        }

        // ----------- ANDS -----------
        case 0b11101010000: {
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;


            int64_t result = CURRENT_STATE.REGS[rn] & CURRENT_STATE.REGS[rm];
            NEXT_STATE.REGS[rd] = result;
            update_flags(result);

            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

         // ----------- EOR -----------

        case 0b11001010000: { 
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;

            
            int64_t result = CURRENT_STATE.REGS[rn] ^ CURRENT_STATE.REGS[rm];
            NEXT_STATE.REGS[rd] = result;

            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        
         // ----------- ORR -----------

        case 0b10101010000: 
        {
            uint32_t rd = instr & 0x1F;
            uint32_t rn = (instr >> 5) & 0x1F;
            uint32_t rm = (instr >> 16) & 0x1F;
            

            int64_t result = CURRENT_STATE.REGS[rn] | CURRENT_STATE.REGS[rm];
            NEXT_STATE.REGS[rd] = result;

            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

         // ----------- MOVZ (hw = 0) -----------

        case 0b11010010100: 
        { 
            uint32_t rd = instr & 0x1F;                 // bits [4:0]: destino
            uint64_t imm16 = (instr >> 5) & 0xFFFF;     // bits [20:5]: inmediato de 16 bits
            uint32_t hw = (instr >> 21) & 0x3;          // bits [22:21]: shift selector (hw)
        
            if (hw == 0) {
                NEXT_STATE.REGS[rd] = imm16;
            }
        
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }

        // ----------- STUR -----------
        
        case 0b11111000000: {
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
        
        case 0b111000000: { // STURB
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
        
        case 0b10111000000: { // STURH
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
        
        case 0b11111000010: { // LDUR
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
        
        case 0b111000010: { // LDURB
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
        
        case 0b10111000010: { // LDURH
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
        case 0b00010100000: 
        {
            // Extraer immediate de 26 bits, shift-left por 2 para agregar los dos ceros
            int32_t imm26 = instr & 0x03FFFFFF;
            int64_t offset = sign_extend(imm26 << 2, 28); // signo extendido a 64 bits

            NEXT_STATE.PC = CURRENT_STATE.PC + offset;
            return;
        }

        // ----------- BR -----------
        case 0b11010110000:
        {
            uint32_t rn = (instr >> 5) & 0x1F; // bits [9:5] — registro fuente

            NEXT_STATE.PC = CURRENT_STATE.REGS[rn];
            return;
        }

        // ----------- B.cond (Branch condicional) -----------
        case 0b01010100000: // B.cond
        {
            int32_t imm19 = (instr >> 5) & 0x7FFFF; // bits [23:5]
            int64_t offset = sign_extend(imm19 << 2, 21); // signed 19-bit offset shifted << 2
            uint32_t cond = instr & 0xF; // bits [3:0]
    
            bool should_branch = false;
    
            switch (cond) {
                case 0x0: // BEQ
                    should_branch = (FLAGS.Z == 1);
                    break;
                case 0x1: // BNE
                    should_branch = (FLAGS.Z == 0);
                    break;
                case 0xA: // BGE
                    should_branch = (FLAGS.N == FLAGS.V);
                    break;
                case 0xB: // BLT
                    should_branch = (FLAGS.N != FLAGS.V);
                    break;
                case 0xC: // BGT
                    should_branch = (FLAGS.Z == 0 && FLAGS.N == FLAGS.V);
                    break;
                case 0xD: // BLE
                    should_branch = (FLAGS.Z == 1 || FLAGS.N != FLAGS.V);
                    break;
                default:
                    // No hacemos nada si es una condición no implementada
                    break;
            }
    
            if (should_branch) {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
    
            return;
        }
    
        case 0b11010100010: {   // HALT: Detener la simulación
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