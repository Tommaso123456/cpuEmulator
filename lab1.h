/*
 * file:        lab1.h
 * description: definitions for Lab 1
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct cpu {
    uint8_t  memory[64*1024];
    uint16_t R[8];
    uint16_t PC;
    uint16_t SP;
    bool Z;
    bool N;
};

/* execute one instruction, return 0 to continue and 1 to halt
 */
extern bool emulate(struct cpu *cpu);
