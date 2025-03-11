#ifndef HELIUM_HEADER
#define HELIUM_HEADER
#define KILOBYTES_RAM 4
#define NUM_REGISTERS 6
#define BYTES_RAM KILOBYTES_RAM * 1024
#define INSTRUCTION_SIZE 9

typedef struct{
    char opcode;
    unsigned int x;
    unsigned int y;
} Instruction;

typedef struct {
    Instruction* instructions;
    int num_instructions;
} VMProgram;

void vm_clock(int cycles);

int vm_instruction(Instruction* instr);

VMProgram parse_file(char* filename);

void load_program(VMProgram* program);

void free_vm_program(VMProgram* program);

void vm_print_memory();

void vm_print_registers();

void vm_print_info();

#endif