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

void testSET(struct cpu *cpu){
    //SET R1 = 0x1234
    zerocpu(cpu);
    store2(cpu, 0x1001, 0);
    store2(cpu, 0x1234, 2);
    int val = emulate(cpu);
    assert(val == 0);
    
    assert(cpu->R[1] == 0x1234);
    assert(cpu->PC == 4);

    printf("--PASSED SET TEST-- \n");
}



void testLOAD(struct cpu *cpu){

    //LOAD R1 <- *R3
    zerocpu(cpu);
    store2(cpu, 0x5678, 0x1234);
    cpu->R[3] = 0x1234;

    store2(cpu, 0x2819, 0);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0x5678);
    assert(cpu->PC == 2);

    //LOAD.B R1 <- *R3
    zerocpu(cpu);
    store2(cpu, 0x5678, 0x1234);
    cpu->R[3] = 0x1234;

    store2(cpu, 0x2C19, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0x0078); //last 8 bits
    assert(cpu->PC == 2);

    //LOAD.B R2 <- *0x5678
    zerocpu(cpu);
    store2(cpu, 0x3456, 0x5678);

    store2(cpu, 0x2402, 0);
    store2(cpu, 0x5678, 2);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[2] == 0x0056); //last 8-bits
    assert(cpu->PC == 4);

    //LOAD R2 <- *0x5678
    zerocpu(cpu);
    store2(cpu, 0x3456, 0x5678);

    store2(cpu, 0x2002, 0);
    store2(cpu, 0x5678, 2);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[2] == 0x3456); //all 16 bits
    assert(cpu->PC == 4);

    printf("--PASSED LOAD TEST-- \n");
}

void testSTORE(struct cpu *cpu){
    zerocpu(cpu);
    //STORE R1 -> *0x5678
    cpu->R[1] = 5;
    store2(cpu, 0x3001, 0);
    store2(cpu, 0x5678, 2);
    int val = emulate(cpu);

    assert(val == 0);
    assert(cpu->memory[0x5678] == 5); //all 16 bits
    assert(cpu->PC == 4);

    zerocpu(cpu);
    //STORE.B R1 -> *0x5678
    cpu->R[1] = 0x3333;
    store2(cpu, 0x3401, 0);
    store2(cpu, 0x5678, 2);
    val = emulate(cpu);

    assert(val == 0);
    assert(cpu->memory[0x5678] == 0x0033); //all 16 bits
    assert(cpu->PC == 4);

    //STORE.B R1 -> *R2
    zerocpu(cpu);
    cpu->R[1] = 200;
    cpu->R[2] =0x3456;
    store2(cpu, 0x3C11, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->memory[0x3456] == 200); //only 8 bits
    assert(cpu->PC == 2);

    //STORE R1 -> *R2
    zerocpu(cpu);
    cpu->R[1] = 200;
    cpu->R[2] =0x3456;
    store2(cpu, 0x3811, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->memory[0x3456] == 200); //only 8 bits
    assert(cpu->PC == 2);

    printf("--PASSED STORE TEST-- \n");
}

void testMOVE(struct cpu *cpu){
    //Move R1 -> R2
    zerocpu(cpu);
    cpu->R[1] = 0x1234;

    store2(cpu, 0x4021, 0); 

    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[2] == 0x1234);
    assert(cpu->PC == 2);

    //Move SP -> R1
    zerocpu(cpu);
    cpu->SP = 0x1234;

    store2(cpu, 0x4018, 0); 

    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0x1234);
    assert(cpu->PC == 2);

     printf("--PASSED MOVE TEST-- \n");
}

void testALU(struct cpu *cpu){
    //test Addition

    //2 + 2 = 4
    zerocpu(cpu);
    cpu->R[3] = 2;
    cpu->R[6] = 2;
    store2(cpu, 0x5073, 0);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 4);
    assert(cpu->Z == false);
    assert(cpu->N == false);

    //2 + (-2) = 0
    //2 + 2
    zerocpu(cpu);

    cpu->R[3] = 2;
    cpu->R[6] = -2;
    store2(cpu, 0x5073, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0);
    assert(cpu->Z == true);
    assert(cpu->N == false);

    // -2 + -2 = -4
    zerocpu(cpu);
    cpu->R[3] = -2;
    cpu->R[6] = -2;
    store2(cpu, 0x5073, 0);
    val = emulate(cpu);
    assert(val == 0);
    uint16_t negativeFour = -4;
    assert(cpu->R[1] == negativeFour);

    assert(cpu->Z == false);
    assert(cpu->N == true);



    //test subtraction

    //5 - 2 = 3
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 2;
    store2(cpu, 0x5273, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 3);
    assert(cpu->Z == false);
    assert(cpu->N == false);

    //5 - 5 = 0
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 5;
    store2(cpu, 0x5273, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0);
    assert(cpu->Z == true);
    assert(cpu->N == false);

    //5 - 6 = 0
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 6;
    store2(cpu, 0x5273, 0);
    val = emulate(cpu);
    assert(val == 0);
    uint16_t negativeOne = -1;
    assert(cpu->R[1] == negativeOne);
    assert(cpu->Z == false);
    assert(cpu->N == true);

    //test and

    //5 & 4 = 4 5473
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 4;
    store2(cpu, 0x5473, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 4);
    assert(cpu->Z == false);
    assert(cpu->N == false);

    //0 & 300 = 0
    zerocpu(cpu);
    cpu->R[3] = 0;
    cpu->R[6] = 300;
    store2(cpu, 0x5473, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0);
    assert(cpu->Z == true);
    assert(cpu->N == false);


    //OR
    //5 | 4 = 5
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 4;
    store2(cpu, 0x5673, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 5);
    assert(cpu->Z == false);
    assert(cpu->N == false);

    //0 | 300 = 300
    zerocpu(cpu);
    cpu->R[3] = 0;
    cpu->R[6] = 300;
    store2(cpu, 0x5673, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 300);
    assert(cpu->Z == false);
    assert(cpu->N == false);

    //XOR 
    // 5 ^ 4 = 1
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 4;
    store2(cpu, 0x5873, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 1);
    assert(cpu->Z == false);
    assert(cpu->N == false);

    // 300 ^ 300 = 0
    zerocpu(cpu);
    cpu->R[3] = 300;
    cpu->R[6] = 300;
    store2(cpu, 0x5873, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0);
    assert(cpu->Z == true);
    assert(cpu->N == false);

    // SHIFTR 

    //200 >> 5 = 6
    zerocpu(cpu);
    cpu->R[3] = 200;
    cpu->R[6] = 5;
    store2(cpu, 0x5A73, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 6);
    assert(cpu->Z == false);
    assert(cpu->N == false);

    //CMP

    //5 - 6 = -1 so nonzero negative
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 6;
    store2(cpu, 0x5C73, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0);
    assert(cpu->Z == false);
    assert(cpu->N == true);

    //5 - 5 = -1 so zero non negative
    zerocpu(cpu);
    cpu->R[3] = 5;
    cpu->R[6] = 5;
    store2(cpu, 0x5C73, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0);
    assert(cpu->Z == true);
    assert(cpu->N == false);

    //Test

    //ra = -1

    zerocpu(cpu);
    cpu->R[3] = -1;
    store2(cpu, 0x5E73, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0);
    assert(cpu->Z == false);
    assert(cpu->N == true);

    //ra = 0

    zerocpu(cpu);
    cpu->R[3] = 0;
    store2(cpu, 0x5E73, 0);
    val = emulate(cpu);
    assert(val == 0);
    assert(cpu->R[1] == 0);
    assert(cpu->Z == true);
    assert(cpu->N == false);

    printf("--PASSED ALU TEST-- \n");
}

void testJUMP(struct cpu *cpu){
  
    //Jump abs 
    zerocpu(cpu);
    store2(cpu, 0x6000, 0);
    store2(cpu, 0x0008, 2);
    int val = emulate(cpu);
    assert(val == 0);
    assert(cpu->PC == 0x0008);


    printf("--PASSED JMP TEST-- \n");
}





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

struct cpu cpu;

int main(int argc, char **argv)
{
    testSET(&cpu);
    testLOAD(&cpu);
    testSTORE(&cpu);
    testMOVE(&cpu);
    testALU(&cpu);
    testJUMP(&cpu);

    test1(&cpu);
    //test2(&cpu);

    printf("all tests PASS\n");
}
