#ifndef PROGRAMS_HEADER

#define PROGRAMS_HEADER
#define INSTRUCTION_SIZE sizeof(Instruction)
#define VM_INSTRUCTION_SIZE 9

typedef struct {
    char opcode;
    unsigned int x;
    unsigned int y;
} Instruction;

typedef struct {
    Instruction* instructions;
    int num_instructions;
    int index;
} VMProgram;

int expand_program(VMProgram* p, int i);

Instruction create_instruction(char opcode, int x, int y);

void mkinstr(VMProgram* p, char opcode, int x, int y);

void addfunc(VMProgram* p, VMProgram* source);

VMProgram* create_program_array();

unsigned char* create_program_ramdisk(VMProgram* program);

void write_ramdisk_to_file(unsigned char* ramdisk, int array_size, char* filename);

#endif