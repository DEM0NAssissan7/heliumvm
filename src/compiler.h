#include "vars.h"
#include "helium.h"

#ifndef COMPILER_H
#define COMPILER_H

extern unsigned char* prgmem;

extern int current_address;

void init_mem();

void write_prgmem_file(char* filename);



void alloc_variable(Var* var);

void alloc_function(Function* f);

void set_hook();

void insert_instructions(VMProgram* p);

#endif