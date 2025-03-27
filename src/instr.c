#include <stdio.h>
#include <stdlib.h>

#include "helium.h"
#include "lib.h"

#include "instr.h"
#include "vars.h"
#include "hmc.h"

// Helper functions
Instruction mkinstr(int opcode, int x, int y) {
    Instruction instr;
    instr.opcode = opcode;
    instr.x = x;
    instr.y = y;
    return instr;
}

int expand_program(VMProgram* p, int i) {
    int retval = p->num_instructions;
    p->num_instructions += i;
    p->instructions = realloc(p->instructions, p->num_instructions * INSTRUCTION_SIZE);
    if(p->instructions == NULL) {
        fprintf(stderr, "Allocation failed\n");
        exit(1);
    }
    // Returns the program size from BEFORE the expansion
    return retval;
}

void append_instructions(VMProgram* source, VMProgram* target) {
    // we need to reallocate the array before appending to it
    
    int index = expand_program(target, source->num_instructions);
    for(int i = 0; i < source->num_instructions; i++) {
        target->instructions[index + i] = source->instructions[i];
    }

    // Source memory cleanup
    source->num_instructions = -1;
    free(source->instructions);
    free(source);
}

VMProgram* create_program_array(int instr_count) {
    VMProgram* retval = malloc(sizeof(VMProgram));
    if(retval == NULL) {
        fprintf(stderr, "Failed to create program array: return pointer allocation failed.\n");
        exit(1);
    }
    retval->instructions = malloc(instr_count * INSTRUCTION_SIZE);
    if(retval->instructions == NULL) {
        fprintf(stderr, "Failed to create program array: instruction allocation failed.\n");
        exit(1);
    }
    retval->num_instructions = instr_count;
    return retval;
}

// Basic instructions

VMProgram* execute(Function* f) {
    /* psuedo code
    set 2 (f.address)
    jmp
    */
   VMProgram* p = create_program_array(2);
   p->instructions[0] = mkinstr(10, 2, f->start_address); // set
   p->instructions[1] = mkinstr(8, 0, 0); // jmp
   return p;
}

VMProgram* twc(int reg) {
    /* Psuedo
    
    mv $register 1
    set 2 1
    flp
    add
    mv 1 $register
    */

    VMProgram* p = create_program_array(5);
    p->instructions[0] = mkinstr(11, reg, 1);
    p->instructions[1] = mkinstr(10, 2, 1);
    p->instructions[2] = mkinstr(2, 0, 0);
    p->instructions[3] = mkinstr(1, 0, 0);
    p->instructions[3] = mkinstr(11, 1, reg);
    return p;
}

VMProgram* set_value(Var* var_left, Var* var_right) {
    /* psuedo code
        ld $var_right.addr[+ i]
        str $var_left.addr[+ i]
    */
    if(!is_same_type(var_left, var_right)) {
        fprintf(stderr, "Variables are not the same type\n");
        return NULL;
    }

    int var_size = varsize(var_left);
    VMProgram* p = create_program_array(var_size * 2);
    for(int i = 0; i < var_size; i++) {
        p->instructions[i*2] = mkinstr(12, var_right->address + i, 0); // ld
        p->instructions[i*2 + 1] = mkinstr(13, var_left->address + i, 0);// str
    }
    return p;
}

VMProgram* set_value_const(Var* var, unsigned int num) {
    /* Psuedo Code
    set 1 $num.1
    str $var.address
    set 1 $num.2
    str $(var.address + 1)
    ...
    */
    int var_size = varsize(var);
    VMProgram* p = create_program_array(var_size * 2);
    unsigned char* split = split_int(num);
    for(int i = 0; i < var_size; i++) {
        p->instructions[i*2] = mkinstr(10, 1, split[i]); // set
        p->instructions[i*2 + 1] = mkinstr(13, var->address + i, 0); // str
    }
    return p;
}

VMProgram* load_variable(Var* var, int reg) {
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
    int var_size = varsize(var);
    int instr_count = 2 + 2 * var_size + 3 * (var_size - 1);
    VMProgram* p = create_program_array(instr_count);
    
    p->instructions[0] = mkinstr(10, 2, 0); // set
    int offset = 1;
    for(int i = 0; i < var_size; i++) {
        int index = i * 5 + offset;
        p->instructions[index + 0] = mkinstr(12, var->address + i, 0); // ld
        p->instructions[index + 1] = mkinstr(1, 0, 0); // add
        if(i + 1 >= var_size) break;
        p->instructions[index + 2] = mkinstr(10, 2, 8); // set
        p->instructions[index + 3] = mkinstr(5, 0, 0); // sl
        p->instructions[index + 4] = mkinstr(11, 1, 2); // mv
    }
    p->instructions[instr_count - 1] = mkinstr(11, 1, reg); // mv to target register
    return p;
}

VMProgram* store_variable(Var* var, int reg) {
    // Take the register and store the variable in memory
    /*
        mv $register 1
        mv $register 3

        # loop
        set 2 ((varsize - i - 1) * 8)
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
    int var_size = varsize(var);
    int instr_count = 2 + 3 * var_size + 6 * (var_size - 1);
    VMProgram* p = create_program_array(instr_count);

    p->instructions[0] = mkinstr(11, reg, 1);
    p->instructions[1] = mkinstr(11, reg, 3);

    int offset = 2;
    for(int i = 0; i < var_size; i++) {
        int index = i * 9 + offset;
        p->instructions[index + 0] = mkinstr(10, 2, (var_size - i - 1) * 8); // set
        p->instructions[index + 1] = mkinstr(6, 0, 0); // sr
        p->instructions[index + 2] = mkinstr(13, var->address + i, 0); // str
        if(i + 1 >= var_size) break;
        p->instructions[index + 3] = mkinstr(10, 2, 8); // set
        p->instructions[index + 4] = mkinstr(5, 0, 0); // sl
        p->instructions[index + 5] = mkinstr(11, 1, 2); // mv
        p->instructions[index + 6] = mkinstr(11, 3, 1); // mv
        p->instructions[index + 7] = mkinstr(2, 0, 0); // sub
        p->instructions[index + 8] = mkinstr(11, 1, 3); // mv
    }
    return p;
}

// Complex functions

VMProgram* var_single_operation(Var* result_var, Var* var1, Var* var2, int operation) {
    VMProgram* p = create_program_array(0);
    if(!is_same_type(var1, var2) || !is_same_type(result_var, var1)) {
        fprintf(stderr, "Operation failed: variables are not the same type\n");
        return NULL;
    }

    append_instructions(load_variable(var1, 4), p);
    append_instructions(load_variable(var2, 5), p);
    

    int after_index = expand_program(p, 4);
    p->instructions[after_index + 0] = mkinstr(11, 4, 1); // mv
    p->instructions[after_index + 1] = mkinstr(11, 5, 2); // mv
    p->instructions[after_index + 2] = mkinstr(operation, 0, 0); // operation
    p->instructions[after_index + 3] = mkinstr(11, 1, 5); // mv

    append_instructions(store_variable(result_var, 5), p);
    return p;
}


VMProgram* add(Var* var_left, Var* var1, Var* var2) {
    return var_single_operation(var_left, var1, var2, 1);
}

VMProgram* subtract(Var* var_left, Var* positive, Var* negative) {
    VMProgram* p = create_program_array(0);
    append_instructions(load_variable(negative, 4), p);
    append_instructions(twc(4), p);
    append_instructions(load_variable(positive, 5), p);
    int index = expand_program(p, 3);
    /* Psuedo code 
    
    mv 4 1
    mv 5 2
    add
    */
    p->instructions[index + 0] = mkinstr(11, 4, 1);
    p->instructions[index + 1] = mkinstr(11, 5, 2);
    p->instructions[index + 2] = mkinstr(11, 5, 2);

    return p;
}

VMProgram* or(Var *var_left, Var* var1, Var* var2) {
    return var_single_operation(var_left, var1, var2, 3);
}

VMProgram* and(Var *var_left, Var* var1, Var* var2) {
    return var_single_operation(var_left, var1, var2, 4);
}

VMProgram* shift_left(Var *var_left, Var* victim, Var* shift_bits) {
    return var_single_operation(var_left, victim, shift_bits, 5);
}

VMProgram* shift_right(Var *var_left, Var* victim, Var* shift_bits) {
    return var_single_operation(var_left, victim, shift_bits, 6);
}

/* Initial program hook 

This is the first line in the ramdisk that specifies where to jump to when the program begins
*/

VMProgram* init_hook(int address) {
    Function f;
    f.start_address = address;
    return execute(&f);
}
void set_hook_pointer(unsigned char* ramdisk, int address) {
    // This function assumes that the init hook is already in the beginning of the ramdisk file
    VMProgram* p = init_hook(address);
    int size = p->num_instructions * VM_INSTRUCTION_SIZE;
    unsigned char* code = create_program_ramdisk(p);
    free(p);
    for(int i = 0; i < size; i++)
        ramdisk[i] = code[i];
    free(code);
}

/* Ending hooks */
VMProgram* halt() {
    VMProgram* p = create_program_array(1);
    p->instructions[0] = mkinstr(14, 0, 0);
    return p;
}