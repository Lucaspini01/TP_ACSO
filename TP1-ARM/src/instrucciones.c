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

//Instrucciones de ARM-V8

void add_immediate(uint32_t instr){
    int32_t rd = instr & 0x1F;             // bits [4:0]
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
}

void adds_extended_register(uint32_t instr) {
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
}

void subs_immediate(uint32_t instr){
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
}

void subs_extended_register(uint32_t instr){
    uint32_t rd = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        uint32_t rm = (instr >> 16) & 0x1F;

        int64_t result = CURRENT_STATE.REGS[rn] - CURRENT_STATE.REGS[rm];

        // Si no es CMP, guardar resultado
        if (rd != XZR) NEXT_STATE.REGS[rd] = result;

        // Update flags
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
    uint32_t rd = instr & 0x1F;                 // bits [4:0]: destino
        uint64_t imm16 = (instr >> 5) & 0xFFFF;     // bits [20:5]: inmediato de 16 bits
        uint32_t hw = (instr >> 21) & 0x3;          // bits [22:21]: shift selector (hw)
    
        if (hw == 0) {
            NEXT_STATE.REGS[rd] = imm16;
        }
    
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void stur(uint32_t instr){
    uint32_t rt = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        int32_t imm9 = (instr >> 12) & 0x1FF;
        imm9 = (imm9 << 23) >> 23;  // Sign-extend 9 bits
    
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
        mem_write_32(addr, byte_val);  // Solo escribimos 1 byte en realidad
    
        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}

void sturh(uint32_t instr){
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
}

void ldur(uint32_t instr){
    uint32_t rt = instr & 0x1F;
        uint32_t rn = (instr >> 5) & 0x1F;
        int32_t imm9 = (instr >> 12) & 0x1FF;
        imm9 = (imm9 << 23) >> 23;  // sign-extend de 9 bits
    
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
    imm9 = (imm9 << 23) >> 23;  // sign-extend 9 bits

    uint64_t addr = CURRENT_STATE.REGS[rn] + imm9;
    uint32_t word = mem_read_32(addr & ~0x3);  // leer la palabra alineada
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
     // Extraer immediate de 26 bits, shift-left por 2 para agregar los dos ceros
     int32_t imm26 = instr & 0x03FFFFFF;
     int64_t offset = sign_extend(imm26 << 2, 28); // signo extendido a 64 bits

     NEXT_STATE.PC = CURRENT_STATE.PC + offset;
}

void br(uint32_t instr){
    uint32_t rn = (instr >> 5) & 0x1F; // bits [9:5] — registro que contiene la dirección
    NEXT_STATE.PC = CURRENT_STATE.REGS[rn]; // salto incondicional
}   

void b_cond(uint32_t instr){
    uint32_t cond = instr & 0xF; // bits [3:0]

        //
        int FLAG_N = CURRENT_STATE.FLAG_N;
        int FLAG_Z = CURRENT_STATE.FLAG_Z;
        int FLAG_C = 0; // No se usa en este caso
        int FLAG_V = 0; // No se usa en este caso
        // Verificar la condición

        bool should_branch = false;

        switch (cond) {
            case 0b0000: // BEQ
                should_branch = (CURRENT_STATE.FLAG_Z == 1);
                break;
            case 0b0001: // BNE
                should_branch = (CURRENT_STATE.FLAG_Z == 0);
                break;
            case 0b1010: // BGE
                should_branch = (CURRENT_STATE.FLAG_N == FLAG_V);
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
                // No hacemos nada si es una condición no implementada
                break;
        }

        if (should_branch) {
            int32_t imm19 = (instr >> 5) & 0x7FFFF; // Extrae el desplazamiento de 19 bits
            int64_t offset = sign_extend(imm19, 19) << 2; // Extiende el signo y ajusta el desplazamiento
    
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
    
    

    // Solo manejamos versión de 64 bits
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

        printf("UBFM: immr=%u, imms=%u\n", immr, imms);

        switch (op_type) {
            case LSL:
                result = value << (64 - immr);
                break;
            case LSR:
                result = value >> immr;
                break;
            case OTHER:
                break; // No hacemos nada si no es LSL ni LSR
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
