#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <stdint.h>

void add_immediate(uint32_t instr);
void add_shifted_register(uint32_t instr);
void adds_immediate(uint32_t instr);
void adds_extended_register(uint32_t instr);
void subs_immediate(uint32_t instr);
void subs_extended_register(uint32_t instr);
void mul(uint32_t instr);
void ands(uint32_t instr);
void eor(uint32_t instr);
void orr(uint32_t instr);
void movz(uint32_t instr);
void stur(uint32_t instr);
void sturb(uint32_t instr);
void sturh(uint32_t instr);
void ldur(uint32_t instr);
void ldurb(uint32_t instr);
void ldurh(uint32_t instr);
void b(uint32_t instr);
void br(uint32_t instr);
void b_cond(uint32_t instr);
void cbz(uint32_t instr);
void cbnz(uint32_t instr);
void lsl_lsr(uint32_t instr);
void hlt(uint32_t instr);

#endif
