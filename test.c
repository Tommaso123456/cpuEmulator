#include <stdio.h>
#include <assert.h>

#include "lab1.h"

/* these functions are already provided in emulate.c
 */
extern void store2(struct cpu *cpu, uint16_t data, uint16_t addr);
extern uint16_t load2(struct cpu *cpu, uint16_t addr);

/* set all registers, flags, and memory to zero
 */
void zerocpu(struct cpu *cpu)
{
    cpu->Z = cpu->N = 0;
    cpu->PC = cpu->SP = 0;
    for (int i = 0; i < 8; i++)
        cpu->R[i] = 0;
    memset(cpu->memory, 0, 64*1024);
}

/* 0000 : 5073      : ADD R3 + R6 -> R1
 */
void test1(struct cpu *cpu)
{
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 10;
    store2(cpu, 0x5073, 0);

    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 15);
}

/* 0002 : 3402 0008 : STORE.B R2 -> *0x0008
 */
void test2(struct cpu *cpu)
{
    zerocpu(cpu);
    cpu->R[2] = 0x1234;
    store2(cpu, 0x3402, 2);
    store2(cpu, 0x0008, 4);

    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[2] == 0x1234);
    assert(cpu->memory[0x0007] == 0); /* not here */
    assert(cpu->memory[0x0008] == 0x34); /* store single byte here */
    assert(cpu->memory[0x0009] == 0); /* not here */
}

char memory[64*1024];
struct cpu cpu;

int main(int argc, char **argv)
{
    cpu.memory = memory;
    
    test1(&cpu);

    printf("all tests PASS\n");
}
