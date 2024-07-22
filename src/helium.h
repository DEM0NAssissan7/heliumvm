#ifndef HELIUM_HEADER
#define HELIUM_HEADER
#define KILOBYTES_RAM 4
#define NUM_REGISTERS 4
#define BYTES_RAM KILOBYTES_RAM * 1024
#define INSTRUCTION_SIZE 9

typedef struct{
    char opcode;
    unsigned int x;
    unsigned int y;
} Instruction;

void vm_clock(int cycles);

int vm_instruction(Instruction* instr);

void load_program(Instruction* program, int num_instructions);

void vm_print_memory();

void vm_print_registers();

void vm_print_info();

#endif