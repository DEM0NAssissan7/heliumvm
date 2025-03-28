#ifndef HMC_LIB
#define HMC_LIB

#include "programs.h"

void print_hmc_program(VMProgram* program);

VMProgram* hmc_parse_file(char *path);

unsigned int* consolidate_char_array(char* array, int size);

void write_ramdisk_to_file(unsigned char* ramdisk, int array_size, char* filename);

#endif