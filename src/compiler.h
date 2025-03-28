#include "vars.h"
#include "programs.h"

#ifndef COMPILER_H
#define COMPILER_H

extern unsigned char* prgmem;

extern int current_address;

void init_mem();

void write_prgmem_file(char* filename);



void alloc_variable(Var* var);

void alloc_function(Function* f);

void set_hook();

void insert_code(VMProgram* p);

void insert_instruction(char opcode, int x, int y);

#endif