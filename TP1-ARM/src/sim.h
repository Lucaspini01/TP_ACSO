#ifndef SIM_H
#define SIM_H

#include <stdint.h>

typedef void (*InstructionFunction)(uint32_t instruction);

typedef struct {
    uint32_t opcode;
    uint32_t mask;
    InstructionFunction execute;
} InstructionEntry;

#include "instrucciones.h"

#endif
