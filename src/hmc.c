#include "hmc.h"
#include "lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int terminated(char c) {
    switch (c)
    {
        case ';':
        case '\n':
            return 1;
    }
    return 0;
}

int unregistered(char c)
{
    switch(c)
    {
        case ' ':
        case '#':
        case ';':
        case '\n':
            return 1;
    }
    return 0;
}

VMProgram* hmc_parse_file(char *path)
{
    Instruction* code = NULL;
    VMProgram* program;
    FILE* fd = fopen(path, "r");
    if(fd == NULL)
    {
        fprintf(stderr, "Failed to open file '%s'", path);
        return NULL;
    }
    char c;
    char* buff = malloc(0);
    int buff_len = 0;
    int i = 0;
    int instructions = 0;
    int line = 1;
    int comment = 0;

    char opcode;
    unsigned int x, y;

    while(1)
    {
        c = fgetc(fd);
        int eof = feof(fd);

        switch(c)
        {
            case '#':
                comment = 1;
                break;
            case '\n':
                comment = 0;
                break;
        }
        if(comment) {
            if(eof) break;
            continue;
        }
        if(c == ' ' || c == '\n' || eof)
        {
            if(buff_len > 0)
            {
                char* operation_string;
                switch(i)
                {
                    case 0:
                        opcode = parse_operation(buff);
                        if(opcode == -1)
                        {
                            fprintf(stderr, "Invalid operation %s; Line %d (%s %d %d)\n", buff, line, buff, x, y);
                            free(buff);
                            return program;
                        }
                        break;
                    case 1:
                        x = parse_int(buff);
                        break;
                    case 2:
                        y = parse_int(buff);
                        break;
                    default:
                        operation_string = parse_opcode(opcode);
                        fprintf(stderr, "Too many arguments; Line %d (%s %d %d)\n", line, operation_string, x, y);
                        return program;
                }
                i++;
                // memset(buff, 0, buff_len);
                buff = malloc(0);
                buff_len = 0;
            }
        }
        if(c == '\n')
            line++;
        if(c == '\n' || c == ';' || eof)
        {
            Instruction instr;
            instr.opcode = opcode;
            instr.x = x;
            instr.y = y;

            instructions++;

            code = realloc(code, sizeof(Instruction) * instructions);
            code[instructions - 1] = instr;

            i = 0;
            opcode = 0;
            x = 0;
            y = 0;
            if(eof) break;
            continue;
        }
        if(c == ' ') continue;
        buff_len++;
        buff = realloc(buff, buff_len);
        buff[buff_len - 1] = c;
    }
    free(buff);
    fclose(fd);
    program = malloc(sizeof(VMProgram) * instructions);
    program->instructions = code;
    program->num_instructions = instructions;
    return program;
}

void free_hmc_program(VMProgram* program)
{
    free(program->instructions);
    free(program);
}

void putint(int i, FILE* fptr) {
    unsigned char* split = split_int(i);
    for(int j = 0; j < 4; j++) {
        fputc(split[j], fptr);
        printf("Split: %d\n", split[j]);
    }
    free(split);
}
void create_machine_code_file(VMProgram* program, char* path) {
    FILE* fptr = fopen(path, "w+");
    Instruction instr;
    for(int i = 0; i < program->num_instructions; i++) {
        instr = program->instructions[i];
        fputc(instr.opcode, fptr);
        fputc(instr.x, fptr);
        fputc(instr.y, fptr);
    }
    fclose(fptr);
}

// Ramdisk creation

unsigned char* create_program_ramdisk(VMProgram* program) {
    unsigned char* ramdisk = malloc(program->num_instructions * INSTRUCTION_SIZE);
    Instruction instr;
    int index;
    unsigned char* splitX;
    unsigned char* splitY;
    for(int i = 0; i < program->num_instructions; i++) {
        instr = program->instructions[i];
        index = i * INSTRUCTION_SIZE;
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
unsigned int* consolidate_char_array(char* array, int size) {
    unsigned int* retval = malloc(size);
    unsigned char* int_parts = malloc(4); // 4 x 4
    for(int i = 0; i < size; i+=4) {
        int_parts[3] = array[i + 0];
        int_parts[2] = array[i + 1];
        int_parts[1] = array[i + 2];
        int_parts[0] = array[i + 3];
        retval[i / 4] = combine_int(int_parts);
    }
    free(int_parts);
    return retval;
}

void write_ramdisk_to_file(unsigned char* ramdisk, int array_size, char* filename) {
    FILE* fptr = fopen(filename, "w+");
    for(int i = 0; i < array_size; i++)
        fputc((int) ramdisk[i], fptr);
    fclose(fptr);
}

void print_hmc_program(VMProgram* program)
{
    // Primarily for spotting compiler issues
    for(int i = 0; i < program->num_instructions; i++)
    {
        Instruction instr = program->instructions[i];
        printf("%d: (%u) %s %u %u\n", i, instr.opcode, parse_opcode(instr.opcode), instr.x, instr.y);
    }
}