#include "helium.h"
#include "lib.h"

#include <stdio.h>

unsigned char mem[BYTES_RAM];
unsigned int reg[NUM_REGISTERS];
unsigned long long pc = 0;
unsigned long long runs = 0;
unsigned long long program_size = 0;

int vm_instruction(Instruction *instr)
{
    char opcode = instr->opcode;
    unsigned int x = instr->x;
    unsigned int y = instr->y;

    switch (opcode)
    {
    case 0: // nul
        return 0;
    case 1: // add
        reg[0] = reg[0] + reg[1];
        break;
    case 2: // sub
        reg[0] = reg[0] - reg[1];
        break;
    case 3: // or
        reg[0] = reg[0] || reg[1];
        break;
    case 4: // and
        reg[0] = reg[0] && reg[1];
        break;
    case 5: // sl
        reg[0] = reg[0] << reg[1];
        break;
    case 6: // sr
        reg[0] = reg[0] >> reg[1];
        break;
    case 7: // lt
        reg[0] = reg[0] < reg[1];
        break;
    case 8: // jmp
        pc = reg[1];
        return 1;
    case 9: // cjp
        if (reg[0])
        {
            pc = reg[1];
            return 1;
        }
        break;
    case 10: // set
        reg[x] = y;
        break;
    case 11: // mv
        reg[y] = reg[x];
        break;
    case 12: // ld
        reg[y] = mem[x];
        break;
    case 13: // str
        mem[x] = reg[0];
        break;
    case 14: // hlt
        printf("Halted program!\n");
        return 2;
        break;

    default:
        printf("Unrecognized instruction %d", opcode);
        break;
    }
    return 0;
}

void load_program(Instruction* program, int num_instructions)
{
    // Perform a check to see if the program will fit in the ram
    if(num_instructions * INSTRUCTION_SIZE > BYTES_RAM) // If the program exceeds the boundaries of the ram
    {
        fprintf(stderr, "Loading program failed. Program of size %d exceeds ram limit of %d bytes.\n", num_instructions * INSTRUCTION_SIZE, BYTES_RAM);
        return;
    }
    for(int i = 0; i < num_instructions; i++) {
        Instruction instr = program[i];

        char opcode = instr.opcode;
        int x = instr.x;
        int y = instr.y;
        int mem_index = i * INSTRUCTION_SIZE;

        mem[mem_index] = opcode;

        unsigned char* split_x = split_int(x); // Split x integer into 4 chars and store in memory
        for(int j = 0; j < 4; j++)
            mem[mem_index + j + 1] = split_x[j];

        unsigned char* split_y = split_int(y); // Split y integer into 4 chars and store in memory
        for(int j = 0; j < 4; j++)
            mem[mem_index + j + 5] = split_y[j];
    }
    program_size = num_instructions * INSTRUCTION_SIZE;
}

void vm_clock(int cycles)
{
    for(int f = 0; f < cycles; f++)
    {
        runs++;
        // What happens in one clock cycle
        int mem_index = pc * INSTRUCTION_SIZE;
        Instruction instr;

        instr.opcode = mem[mem_index]; // Get opcode

        // Combine the next 4 address spaces into a single int x
        unsigned char x_parts[4];
        unsigned char y_parts[4];
        for(int i = 0; i < 4; i++) {
            x_parts[i] = mem[mem_index + i + 1];
            y_parts[i] = mem[mem_index + i + 5];
        }
        instr.x = combine_int(x_parts);
        instr.y = combine_int(y_parts);

        int flag = vm_instruction(&instr);

        switch(flag)
        {
            case 0:
                pc++;
                break;
            case 1:
                continue;
            case 2:
                return;
        }
    }
}

void vm_print_memory()
{
    for(int i = 0; i < BYTES_RAM; i++)
        printf("%d > %d\n", i, mem[i]);
}

void vm_print_registers()
{
    for(int i = 0; i < NUM_REGISTERS; i++)
        printf("Register %d: %d\n", i, reg[i]);
}

void vm_print_info()
{
    printf("PC: %llu\nRuns: %llu\nProgram Size: %llu bytes (%llu instructions)\nRAM: %dKB (%d bytes)\nRegisters: %d\n",  pc,
                                                                            runs,
                                                                            program_size,
                                                                            program_size / INSTRUCTION_SIZE,
                                                                            KILOBYTES_RAM,
                                                                            BYTES_RAM,
                                                                            NUM_REGISTERS);
}