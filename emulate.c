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
                    uint16_t addy = cpu->R[regB]; //read address from register B
                    uint8_t data =  load2(cpu, addy);  //Read data from register B
                    cpu->R[regA] = data;
                }
                else{
                    int regB = (insn >> 3) & 7; // Register B
                    uint16_t addy = cpu->R[regB]; //read address from register B
                    uint16_t data =  load2(cpu, addy);  //Read data from address from register B
                    cpu->R[regA] = data;
                }
            }
            else{
                if (is_byte){
                    uint16_t addy = load2(cpu, cpu->PC + 2);
                    uint8_t data = load2(cpu, addy); //Read data from specified addr
                    cpu->R[regA] = data;
                        
                }
                else{
                    uint16_t addy = load2(cpu, cpu->PC + 2);
                    uint16_t data = load2(cpu, addy); //Read data from specified addr
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
            return false;
        }

        case 0x4: { //MOVE
            int regFrom = insn & 15; 
            int regTo = (insn >> 4) & 15; 
            if (regFrom == 8){
                uint16_t data = cpu->SP;
                if (regTo == 8){
                    cpu->SP = data;
                }
                else{
                    cpu->R[regTo] = data;
                }
            }
            else if (regFrom < 8){
                uint16_t data = cpu->R[regFrom];
                if (regTo == 8){
                    cpu->SP = data;
                }
                else{
                    cpu->R[regTo] = data;
                }
            }
            else{
                exit(0);
            }

            cpu->PC += 2;
            return false;

        }

        case 0x5: {  //ALU
            int a = insn & 7;
            int b = (insn >> 3) & 7; 
            int c = (insn >> 6) & 7; 
            uint16_t initialA = cpu->R[a];
            uint16_t val = 0;


            if ((insn & 0x0E00) == 0x0000) /* ADD */{
                val = cpu->R[a] + cpu->R[b];
            }
            if ((insn & 0x0E00) == 0x0200 || (insn & 0x0E00) == 0x0C00) /* SUB OR CMP*/{
                val = cpu->R[a] - cpu->R[b];
            }
            if ((insn & 0x0E00) == 0x0400) /* AND */{
                val = cpu->R[a] & cpu->R[b];
            }
            if ((insn & 0x0E00) == 0x0600) /* OR */{
                val = cpu->R[a] | cpu->R[b];
            }
            if ((insn & 0x0E00) == 0x0800) /* XOR */{
                val = cpu->R[a] ^ cpu->R[b];
            }
            if ((insn & 0x0E00) == 0x0A00) /* SHIFT right */{
                val = cpu->R[a] >> cpu->R[b];
            }
            cpu->R[c] = val;
            cpu->N = (val & 0x8000) != 0;
            cpu->Z = (val == 0);
        

            if ((insn & 0x0E00) == 0x0C00) /* CMP */{
                cpu->R[c] = 0; //discard the value and keep the flags
            }
            if ((insn & 0x0E00) == 0x0E00) /* TEST */{
                cpu->R[a] = initialA;
                cpu->N = (cpu->R[a] & 0x8000) != 0; //set flag according to Ra
                cpu->Z = (cpu->R[a] == 0);
                
                
            }
            cpu->PC += 2;
            return false;
        }



        case 0x6: case 0x7: { //JMP
            uint16_t jmpCase = (insn >> 9) & 7;
            bool makeJump = true;
        
            if (jmpCase == 0){
                makeJump = true;
            }
            else if(jmpCase == 1){
                makeJump = (cpu->Z == true);
            }
            else if(jmpCase == 2){
                makeJump = (cpu->Z == false);
            }
            else if(jmpCase == 3){
                makeJump = (cpu->N == true);
            }
            else if(jmpCase == 4){
                makeJump = (cpu->Z == false) & (cpu->N == false);
            }
            else if(jmpCase == 5){
                makeJump = (cpu->Z == true) | (cpu->N == true);
            }
            else if(jmpCase == 6){
                makeJump = (cpu->N == false);
            }
            else {
                    exit(0);
                    printf("aborting");
            }


            if (makeJump){
                if (insn >> 12 & 1){
                    uint16_t regA = insn & 7;
                    cpu->PC = cpu->R[regA];
                } 
                else{
                    uint16_t newVal = load2(cpu, cpu->PC + 2);
                    cpu->PC = newVal;
                }
            }

            if (!makeJump){
                if (insn >> 12 & 1){
                    cpu->PC += 2;
                } 
                else{
                    cpu->PC += 4;
                }
            }

            return false;
        }

        case 0x8: {         //CALL to address in 3rd and 4th bytes
            cpu->SP -= 2;  //Decrease stack pointer
            store2(cpu, cpu->PC + 4 ,cpu->SP); //store value of (PC + 4) in the address in sp
            uint16_t newAddr = load2(cpu, cpu->PC + 2); //read values from 3rd and 4th byte
            cpu->PC = newAddr;

            return false;
        }

        case 0x9: {        //CALL to address in register
            cpu->SP -= 2;
            store2(cpu, cpu->PC + 2,cpu->SP); //store value of (PC + 2) in the address in sp
            int a = insn & 7;
            uint16_t newAddr = cpu->R[a]; //Set PC register equal to 16-bit value in R[A]
            cpu ->PC = newAddr;
            return false;
        }

        case 0xA: { //RET
            cpu->PC = load2(cpu, cpu->SP);
            cpu->SP += 2;

            return false;
        }

        case 0xB: {  //PUSH
            cpu->SP -= 2;
            int a = insn & 7;
            uint16_t data = cpu->R[a];
            uint16_t addr =cpu->SP;
            store2(cpu, data, addr);
            cpu->PC += 2;
            return false;
        }

        case 0xC: { //POP
            uint16_t data = load2(cpu, cpu->SP);
            int a = insn & 7;
            cpu->R[a] = data;
            cpu->SP += 2;
            cpu->PC += 2;

            return false;
        }

        case 0xD: {  //in
            int a = insn & 7;
            cpu->R[a] = fgetc(stdin);
            cpu->PC += 2;

            return false;
        }
        

        case 0xE: {  //out
            int a = insn & 7;
            fputc(cpu->R[a], stdout);
            cpu->PC += 2;
            return false;

        }
        
        default:
            printf("Unknown opcode: 0x%x\n", opcode);
            return true; // Halt on unknown instruction
    }
}

