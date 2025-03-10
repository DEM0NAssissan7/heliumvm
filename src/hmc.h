#ifndef HMC_LIB
#define HMC_LIB
#include "helium.h"

typedef struct{
    Instruction* code;
    int instructions;
} HmcProgram;

void hmc_load_file(char* path);

void print_hmc_program(char *path);

#endif