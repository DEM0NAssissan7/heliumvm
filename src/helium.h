#ifndef HELIUM_HEADER

#define HELIUM_HEADER
#define KILOBYTES_RAM 32
#define NUM_REGISTERS 8
#define BYTES_RAM KILOBYTES_RAM * 1024

#include "programs.h"

typedef struct {
    unsigned char* data;
    int byte_size;
} Ramdisk;

void vm_clock(int cycles);

int vm_instruction(Instruction* instr);

Ramdisk parse_file_ramdisk(char* filename);

void load_ramdisk(unsigned char* ramdisk, int length);

void vm_print_memory();

void vm_print_registers();

void vm_print_info();

#endif