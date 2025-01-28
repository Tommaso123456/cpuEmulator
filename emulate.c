/*
 * file:        emulate-soln.c
 * description: solution to Lab 1
 */

#include <stdio.h>
#include <stdlib.h>

#include "lab1.h"

void store2(struct cpu *cpu, uint16_t data, uint16_t addr) {
    cpu->memory[addr] = data & 0xFF;
    cpu->memory[addr+1] = (data >> 8) & 0xFF;
}

uint16_t load2(struct cpu *cpu, uint16_t addr) {
    return (cpu->memory[addr] | (cpu->memory[addr+1] << 8));
}


/* emulate(struct cpu*) - emulate a single instruction
 *     - returns 1 for halt, 0 for no halt 
 */
bool emulate(struct cpu *cpu)
{
    //load instruction from memory
    uint16_t insn = load2(cpu, cpu->PC);

    // Skip the first 12 bits to get to the last 4 (the opcode)
    uint16_t opcode = (insn >> 12) & 0xF;

    // Use a switchcase for the opcodes
    switch (opcode) {
        case 0x1: { // SET
            int reg = insn & 0x7; // Extract destination register
            uint16_t value = load2(cpu, cpu->PC + 2); // Load value
            cpu->R[reg] = value; // Set register
            cpu->PC += 4; // Increment PC
            return false;
        }
        case 0xF: { // HALT
            return true; // Stop emulation
        }
        case 0x2: { // LOAD
            int reg = (insn >> 9) & 0x7; // Destination register
            int is_byte = (insn >> 10) & 0x1; // Byte or word flag
            int is_indirect = (insn >> 11) & 0x1; // Address type

            uint16_t addr;
            if (is_indirect) {
                int src_reg = insn & 0x7; // Source register
                addr = cpu->R[src_reg];
            } else {
                addr = load2(cpu, cpu->PC + 2); // Constant address
            }

            if (is_byte) {
                cpu->R[reg] = cpu->memory[addr];
            } else {
                cpu->R[reg] = load2(cpu, addr);
            }
            cpu->PC += is_indirect ? 2 : 4; // Update PC
            return false;
        }
        // Implement other opcodes (e.g., STORE, MOVE, ADD, SUB, etc.)


        
        default:
            printf("Unknown opcode: 0x%x\n", opcode);
            return true; // Halt on unknown instruction
}

