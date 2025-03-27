#ifndef HMC_LIB
#define HMC_LIB
#include "helium.h"

void print_hmc_program(VMProgram* program);

VMProgram* hmc_parse_file(char *path);

unsigned char* create_program_ramdisk(VMProgram* program);

unsigned int* consolidate_char_array(char* array, int size);

void write_ramdisk_to_file(unsigned char* ramdisk, int array_size, char* filename);

#endif