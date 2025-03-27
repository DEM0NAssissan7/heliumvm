#include "helium.h"
#include "lib.h"

#include <stdio.h>
#include <stdlib.h>

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
    reg[0] = pc; // Readonly pc register

    switch (opcode)
    {
    case 0: // nul
        return 0;
    case 1: // add
        reg[1] = reg[1] + reg[2];
        break;
    case 2: // flp
        reg[1] = ~reg[1];
        break;
    case 3: // or
        reg[1] = reg[1] || reg[2];
        break;
    case 4: // and
        reg[1] = reg[1] && reg[2];
        break;
    case 5: // sl
        reg[1] = reg[1] << reg[2];
        break;
    case 6: // sr
        reg[1] = reg[1] >> reg[2];
        break;
    case 7: // lt
        reg[1] = reg[1] < reg[2];
        break;
    case 8: // jmp
        pc = reg[2];
        return 1;
    case 9: // cjp
        if (reg[1])
        {
            pc = reg[2];
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
        reg[1] = mem[x];
        break;
    case 13: // str
        mem[x] = reg[1];
        break;
    case 14: // hlt
        printf("Halted program!\n");
        return 2;
        break;

    default:
        fprintf(stderr, "Unrecognized instruction %d", opcode);
        break;
    }
    return 0;
}

void add_instr(Instruction* instructions, int num_instructions, Instruction instr) {
}

void load_ramdisk(unsigned char* ramdisk, int length) {
    if(length > BYTES_RAM) {
        fprintf(stderr, "Cannot load ramdisk: not enough ram (ramdisk: %d, VM ram: %d)\n", length, BYTES_RAM);
    }
    for(int i = 0; i < length; i++)
        mem[i] = ramdisk[i];
}
unsigned char* expand_consolidated(unsigned int* array, int byte_size) {
    unsigned char* retval = malloc(byte_size);
    unsigned char* split = malloc(4);
    for(int i = 0; i < byte_size; i+=4) {
        split = split_int(array[i / 4]);
        retval[i + 3] = split[0];
        retval[i + 2] = split[1];
        retval[i + 1] = split[2];
        retval[i + 0] = split[3];
    }
    free(split);
    return retval;
}

Ramdisk parse_file_ramdisk(char* filename) {
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("File %s was unable to be opened. Exiting...\n", filename);
        exit(1);
    }
    int c;
    int i = 0;

    unsigned char* buff = malloc(0);
    while((c = fgetc(fptr)) != EOF) {
        buff = realloc(buff, (i + 1) * 4);
        buff[i] = c;
        i++;
    }
    fclose(fptr);
    // unsigned char* memdisk = expand_consolidated(buff, i * 4);
    Ramdisk retval;
    // retval.data = memdisk;
    retval.data = buff;
    retval.byte_size = i * 4;
    return retval;
}

VMProgram parse_file(char* filename) {
    FILE* fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf("File %s was unable to be opened. Exiting...\n", filename);
        exit(1);
    }
    int c;
    unsigned char parsed_c;
    int i = 0;
    int part = 0;

    Instruction* instructions = malloc(INSTRUCTION_SIZE);
    Instruction instr;
    int num_instructions = 0;
    size_t instruction_size = sizeof(instr);
    while((c = fgetc(fptr)) != EOF) {
        switch(i) {
            case 0:
                instr.opcode = c;
                break;
            case 1:
                instr.x = c;
                break;
            case 2:
                instr.y = c;
                num_instructions++;
                Instruction* p = realloc(instructions, num_instructions * instruction_size);
                if(!p) {
                    fprintf(stderr, "Instructions allocations failed. Terminating program.\n");
                    free(instructions);
                    fclose(fptr);
                    exit(1);
                } else {
                    instructions = p;
                    instructions[num_instructions - 1] = instr;
                }
                i = -1;
                break;
        }
        i++;
    }
    fclose(fptr);
    VMProgram program;
    program.instructions = instructions;
    program.num_instructions = num_instructions;
    return program;
}

void load_program(VMProgram* program)
{
    int num_instructions = program->num_instructions;
    Instruction* instructions = program->instructions;

    // Perform a check to see if the program will fit in the ram
    if(num_instructions * INSTRUCTION_SIZE > BYTES_RAM) // If the program exceeds the boundaries of the ram
    {
        fprintf(stderr, "Loading program failed. Program of size %d exceeds ram limit of %d bytes.\n", num_instructions * INSTRUCTION_SIZE, BYTES_RAM);
        return;
    }
    for(int i = 0; i < num_instructions; i++) {
        Instruction instr = instructions[i];

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

void free_vm_program(VMProgram* program) {
    free(program->instructions);
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