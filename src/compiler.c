/* Beginning of program memory allocation.

This is where variables are given their addresses and where the 
program code is compiled and spliced onto the program file */


/* Program flow



1. Read code file and classify variable and function definitions
    -> Functions are essentially their own program entirely
2. Take all variable definitions and create their address in memory. Assign
them statically if needed.
3. Take all functions (and the main code body) and create the instruction set.
4. Convert the instruction set into binary.
5. Append proper jump at the beginning of the binary so the program
starts execution at the proper place

*/


/*    Memory structurization library

These functions provide the necessary functionality and management of
variable memory
*/

#include "compiler.h"
#include "vars.h"
#include "instr.h"
#include "helium.h"
#include "hmc.h"

#include <stdlib.h>
#include <stdio.h>

int current_address = 0;
int buff_size = 0;
unsigned char* prgmem;

void init_mem() {
    prgmem = malloc(0);
    insert_instructions(init_hook(0));
}

void write_prgmem_file(char* filename) {
    write_ramdisk_to_file(prgmem, buff_size, filename);
}

// Helper functions

void expand_buff(int amount) {
    prgmem = realloc(prgmem, buff_size + amount);
    if(prgmem == NULL) {
        fprintf(stderr, "Buffer expansion failed.\n");
        exit(1);
    }
    buff_size+=amount;
}

/* Allocations */

void alloc_variable(Var* var) {
    int size = varsize(var);
    expand_buff(size);
    var->address = current_address;
    current_address += size;
}

void alloc_function(Function* f) {
    alloc_variable(&f->return_var);
    for(int i = 0; i < f->num_args; i++) {
        alloc_variable(&f->args[i]);
    }
    f->start_address = current_address;
    insert_instructions(&f->program);
}

void set_hook() {
    set_hook_pointer(prgmem, current_address);
}

void insert_instructions(VMProgram* p) {
    int size = p->num_instructions * VM_INSTRUCTION_SIZE;
    unsigned char* code = create_program_ramdisk(p);
    free(p->instructions);
    free(p);
    expand_buff(size);

    for(int i = 0; i < size; i++) {
        prgmem[current_address + i] = code[i];
    }
    free(code);
    current_address += size;
}