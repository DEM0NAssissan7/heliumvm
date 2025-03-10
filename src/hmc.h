#ifndef HMC_LIB
#define HMC_LIB
#include "helium.h"

void print_hmc_program(VMProgram* program);

VMProgram* hmc_parse_file(char *path);

void create_machine_code_file(VMProgram* program, char* path);

#endif