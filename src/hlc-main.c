#include <stdio.h>

#include "compiler.h"
#include "vars.h"
#include "instr.h"

int main() {
    init_mem();

    Var x;
    x.type = Int;
    x.is_signed = 0;
    x.pointer = 0;
    x.name = "x";
    alloc_variable(&x);
    
    Var y;
    y.type = Int;
    y.is_signed = 0;
    y.pointer = 0;
    y.name = "y";
    alloc_variable(&y);

    Var z;
    z.type = Int;
    z.is_signed = 0;
    z.pointer = 0;
    z.name = "z";
    alloc_variable(&z);

    set_hook();
    int h = current_address;

    insert_code(set_value_const(&x, 22));
    insert_code(set_value_const(&y, 15));
    insert_code(subtract(&x, &x, &y));

    insert_code(set_value_const(&z, 129));
    insert_code(subtract(&z, &z, &y));

    insert_code(add(&x, &z, &x));

    insert_code(halt());
    printf("x address: %d, y address %d, hook: %d \n", x.address, y.address, h);
    printf("instruction count: %d\n", (current_address - h) / VM_INSTRUCTION_SIZE + 2);

    write_prgmem_file("lcramdisk");
    return 0;
}