#include "hmc.h"
#include "helium.h"
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

HmcProgram* hmc_parse_file(char *path)
{
    Instruction* code = NULL;
    HmcProgram* program;
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
    program = malloc(sizeof(HmcProgram) * instructions);
    program->code = code;
    program->instructions = instructions;
    return program;
}

void free_hmc_program(HmcProgram* program)
{
    free(program->code);
    free(program);
}

void load_hmc_program(HmcProgram* program)
{
    load_program(program->code, program->instructions);
    free_hmc_program(program);
}

void hmc_load_file(char* path)
{
    HmcProgram* program = hmc_parse_file(path);
    load_hmc_program(program);
}

void print_hmc_program(char* path)
{
    // Primarily for spotting compiler issues
    HmcProgram* program = hmc_parse_file(path);
    for(int i = 0; i < program->instructions; i++)
    {
        Instruction instr = program->code[i];
        printf("%d: (%u) %s %u %u\n", i, instr.opcode, parse_opcode(instr.opcode), instr.x, instr.y);
    }
    free_hmc_program(program);
}