/*
 * file:        lab1.h
 * description: definitions for Lab 1
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct cpu {
    uint8_t  memory[64*1024]; //memory
    uint16_t R[8]; //registers
    uint16_t PC;  //program counter
    uint16_t SP;  //stack pointer
    bool Z;   //zero flag
    bool N;   //negative flag
};

/* execute one instruction, return 0 to continue and 1 to halt
 */
extern bool emulate(struct cpu *cpu);
