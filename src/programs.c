#include <stdio.h>
#include <stdlib.h>

#include "programs.h"
#include "lib.h"

// Helper functions
int expand_program(VMProgram* p, int i) {
    int retval = p->num_instructions;
    p->num_instructions += i;
    p->instructions = realloc(p->instructions, p->num_instructions * INSTRUCTION_SIZE);
    if(p->instructions == NULL) {
        fprintf(stderr, "Allocation failed\n");
        exit(1);
    }
    // Returns the program size from BEFORE the expansion
    return retval;
}

Instruction create_instruction(char opcode, int x, int y) {
    Instruction instr;
    instr.opcode = opcode;
    instr.x = x;
    instr.y = y;
    return instr;
}

void mkinstr(VMProgram* p, char opcode, int x, int y) {
    int index = expand_program(p, 1);
    p->instructions[index] = create_instruction(opcode, x, y);
}

void addfunc(VMProgram* p, VMProgram* source) {
    // we need to reallocate the array before appending to it
    
    int index = expand_program(p, source->num_instructions);
    for(int i = 0; i < source->num_instructions; i++) {
        p->instructions[index + i] = source->instructions[i];
    }

    // Source cleanup
    source->num_instructions = -1;
    free(source->instructions);
    free(source);
}

VMProgram* create_program_array() {
    VMProgram* retval = malloc(sizeof(VMProgram));
    if(retval == NULL) {
        fprintf(stderr, "Failed to create program array: return pointer allocation failed.\n");
        exit(1);
    }
    retval->instructions = malloc(0);
    if(retval->instructions == NULL) {
        fprintf(stderr, "Failed to create program array: instruction allocation failed.\n");
        exit(1);
    }
    retval->num_instructions = 0;
    return retval;
}

// Ramdisk creation

unsigned char* create_program_ramdisk(VMProgram* program) {
    unsigned char* ramdisk = calloc(program->num_instructions, VM_INSTRUCTION_SIZE);
    Instruction instr;
    int index;
    unsigned char* splitX;
    unsigned char* splitY;
    for(int i = 0; i < program->num_instructions; i++) {
        instr = program->instructions[i];
        index = i * VM_INSTRUCTION_SIZE;
        ramdisk[index + 0] = instr.opcode;
        splitX = split_int(instr.x);
        ramdisk[index + 1] = splitX[0];
        ramdisk[index + 2] = splitX[1];
        ramdisk[index + 3] = splitX[2];
        ramdisk[index + 4] = splitX[3];
        splitY = split_int(instr.y);
        ramdisk[index + 5] = splitY[0];
        ramdisk[index + 6] = splitY[1];
        ramdisk[index + 7] = splitY[2];
        ramdisk[index + 8] = splitY[3];
        free(splitX);
        free(splitY);
    }
    return ramdisk;
}
void write_ramdisk_to_file(unsigned char* ramdisk, int array_size, char* filename) {
    FILE* fptr = fopen(filename, "w+");
    for(int i = 0; i < array_size; i++)
        fputc((int) ramdisk[i], fptr);
    fclose(fptr);
}