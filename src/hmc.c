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

void print_hmc_program(VMProgram* program)
{
    // Primarily for spotting compiler issues
    for(int i = 0; i < program->num_instructions; i++)
    {
        Instruction instr = program->instructions[i];
        printf("%d: (%u) %s %u %u\n", i, instr.opcode, parse_opcode(instr.opcode), instr.x, instr.y);
    }
}