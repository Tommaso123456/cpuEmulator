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
    uint16_t opcode = (insn >> 12);
    

    // Switch case to go through opcodes
    switch (opcode) {

        case 0xF: { // HALT
            return true; 
        }

        case 0x1: { // SET
            int reg = insn & 7; // Extract destination register
            uint16_t toStore = load2(cpu, cpu->PC + 2); //Load the value to store from PC + 2 and PC + 3
            cpu->R[reg] = toStore; // Set register
            cpu->PC += 4; // Increment PC
            return false;
        }

        case 0x2: { // LOAD
            
            int regA = insn & 7; // Destination register A
            
            int is_byte = (insn >> 10) & 1; // Byte or word flag
            int is_indirect = (insn >> 11) & 0x1; // constant address or address in register
            // 1 = indirect
            // 0 = direct

            if (is_indirect) {
                if (is_byte){
                    int regB = (insn >> 3) & 7; // Register B
                    uint8_t data = cpu->R[regB]; //Read data from register B
                    cpu->R[regA] = data;
                }
                else{
                    int regB = (insn >> 3) & 7; // Register B
                    uint16_t data = cpu->R[regB]; //Read data from register B
                    cpu->R[regA] = data;
                }
            }
            else{
                if (is_byte){
                    uint8_t data = load2(cpu, cpu->PC + 2); //Read data from specified addr
                    cpu->R[regA] = data;
                        
                }
                else{
                    uint16_t data = load2(cpu, cpu->PC + 2); //Read data from specified addr
                    cpu->R[regA] = data;
                }
            }

            cpu->PC += is_indirect ? 2 : 4; // Update PC
            return false;
        }




         case 0x3: {  //STORE
            int is_byte = (insn >> 10) & 0x1; // Byte or word flag
            int is_indirect = (insn >> 11) & 0x1; // Address type
            //1 = indirect = from another register
            //0 = direct = from address
            int regA = insn & 7; 
            if (is_indirect){
                int regB = (insn >> 3) & 7; 
                if (is_byte){
                    uint8_t data = cpu->R[regA];
                    uint16_t addy = cpu->R[regB]; 
                    store2(cpu, data, addy);
                }
                else{
                    uint16_t data = cpu->R[regA];
                    uint16_t addy = cpu->R[regB];
                    store2(cpu, data, addy);
                }
            }
            else{
                if (is_byte){
                    uint8_t data = cpu->R[regA];
                    uint16_t addy = load2(cpu, cpu->PC + 2);
                    store2(cpu, data, addy);
                }
                else {
                    uint16_t data = cpu->R[regA];
                    uint16_t addy = load2(cpu, cpu->PC + 2);
                    store2(cpu, data, addy);
                }
            }

            cpu->PC += is_indirect ? 2 : 4; // Update PC

         }

        //MOVE

        //ALU

        //JMP_ABS

        //JMP_IND

        //CALL to address in 3rd and 4th bytes

        //CALL to address in register

        //RET

        //PUSH

        //POP

        //IN

        //OUT


        
        default:
            printf("Unknown opcode: 0x%x\n", opcode);
            return true; // Halt on unknown instruction
    }
}

