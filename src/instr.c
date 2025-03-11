#include <stdio.h>

#include "helium.h"
#include "lib.h"

#include "instr.h"
#include "vars.h"

int num_instructions = 0;

// Basic instructions

Instruction* execute(Function f) {

}

Instruction* set_value(Var* var_left, Var* var_right) {
    /* psuedo code
        ld $var_right.addr[+ i]
        str $var_left.addr[+ i]
    */
    if(!is_same_type(var_left, var_right)) {
        fprintf(stderr, "Variables are not the same type\n");
        return NULL;
    }

    int size = varsize(var_left);
    int byte_size = size * 2;
    Instruction* instructions = malloc(byte_size);
    for(int i = 0; i < size; i++) {
        Instruction instr;
        instr.opcode = 12; // ld
        instr.x = var_right->address + i; // $var_left.addr + i
        instr.y = 0; // null
        instructions[i*2] = instr;

        instr.opcode = 13; // str
        instr.x = var_left->address + i; // $(var.address + 1)
        instr.y = 0; // null
        instructions[i*2 + 1] = instr;
    }
    num_instructions += byte_size;
    return instructions;
}

Instruction* set_value_const(Var* var, unsigned int num) {
    /* Psuedo Code
    set $num.1 1
    str $var.address
    set $num.2 1
    str $(var.address + 1)
    ...
    */
    int size = varsize(var);
    int byte_size = size * 2;
    Instruction* instructions = malloc(byte_size);
    unsigned char* split = split_int(num);
    for(int i = 0; i < size; i++) {
        Instruction instr;
        instr.opcode = 10; // set
        instr.x = split[i]; // $num.i
        instr.y = 1; // reg 1
        instructions[i*2] = instr;

        instr.opcode = 13; // str
        instr.x = var->address + i; // $(var.address + 1)
        instr.y = 0; // null
        instructions[i*2 + 1] = instr;
    }
    num_instructions += byte_size;
    return instructions;
}

Instruction* load_variable(Var var, int register) {
    // Grab a variable from memory and store it into a register
    /* psuedo code
    set 2 0

    # begin loop
    ld $(var.address + i)
    add # register 1 is the current sum now

    # if we are going to loop again
    set 2 8
    sl
    mv 1 2 # move to register 2 so we can add it again
    ...

    # at the end
    mv 1 $register
    ...
    */
}

Instruction* store_variable(Var var, int register) {
    // Take the register and store the variable in memory
    /*
        mv $register 1
        mv $register 3

        # loop
        set 2 ((size - i - 1) * 8)
        sr
        str $var.address[i]

        # if we loop again
        set 2 8
        sl
        mv 1 2
        mv 3 1
        sub
        mv 1 3
        ...
    */
}

Instruction* minus(Var var_left, Var var_right, Var factor) {
    
}