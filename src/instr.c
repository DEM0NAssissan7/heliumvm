#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "lib.h"
#include "instr.h"
#include "vars.h"
#include "hmc.h"
#include "programs.h"

// Basic instructions

VMProgram* execute(Function* f) {
   VMProgram* p = create_program_array();

   mkinstr(p, SET, 2, f->start_address);
   mkinstr(p, JMP, 0, 0);
   return p;
}

VMProgram* twc1() { // Mutates 2, 1 -> 1
    VMProgram* p = create_program_array();

    mkinstr(p, FLP, 0, 0);
    mkinstr(p, SET, 2, 1);
    mkinstr(p, ADD, 0, 0);
    return p;

}
VMProgram* twc(int reg) { // Mutates 1, 2 -> reg
    VMProgram* p = create_program_array();

    mkinstr(p, MV, reg, 1);
    addfunc(p, twc1());
    mkinstr(p, MV, 1, reg);
    return p;
}
VMProgram* sub_registers(int reg_positive, int reg_negative) { // reg[1] = reg[reg_positive] - reg[reg_negative]
    // Mutates 1, 2 -> 1
    VMProgram* p = create_program_array();

    mkinstr(p, MV, reg_positive, 1);
    addfunc(p, twc1());
    mkinstr(p, MV, reg_negative, 2);
    mkinstr(p, ADD, 0, 0);
    addfunc(p, twc1()); // Flip back output to normal from 2's
    // Result is in register 1
    return p;
}

VMProgram* set_value(Var* var_left, Var* var_right) {
    if(!is_same_type(var_left, var_right)) {
        fprintf(stderr, "Variables are not the same type\n");
        return NULL;
    }

    int var_size = varsize(var_left);
    VMProgram* p = create_program_array();
    for(int i = 0; i < var_size; i++) {
        mkinstr(p, LD, var_right->address + i, 0); // ld
        mkinstr(p, STR, var_left->address + i, 0);// str
    }
    return p;
}

VMProgram* set_value_const(Var* var, int num) {
    VMProgram* p = create_program_array();

    mkinstr(p, SET, 5, num);
    addfunc(p, store_variable(var, 5));

    // int var_size = varsize(var);
    // unsigned char* split = split_int(num);
    // for(int i = 0; i < var_size; i++) {
    //     mkinstr(p, SET, 1, split[i]); // set
    //     mkinstr(p, STR, var->address + i, 0); // str
    // }
    return p;
}

VMProgram* load_variable(Var* var, int reg) {
    // Grab a variable from memory and store it into a register

    int var_size = varsize(var);
    int instr_count = 2 + 2 * var_size + 3 * (var_size - 1);
    VMProgram* p = create_program_array();
    

    mkinstr(p, SET, 2, 0);
    for(int i = 0; i < var_size; i++) {
        mkinstr(p, LD, var->address + i, 0);
        mkinstr(p, ADD, 0, 0); // register 1 is the current sum now
        if(i + 1 >= var_size) break;
        mkinstr(p, SET, 2, 8);
        mkinstr(p, SL, 0, 0);
        mkinstr(p, MV, 1, 2); // move to register 2 so we can add it again
    }
    mkinstr(p, MV, 1, reg); // mv to target register
    return p;
}

VMProgram* store_variable(Var* var, int reg) {
    // Mutates 1,2,3
    // Take the register and store the variable in memory
    int size = varsize(var);
    VMProgram* p = create_program_array();

    mkinstr(p, MV, reg, 1);
    mkinstr(p, MV, reg, 3);

    int shift;
    for(int i = 0; i < size; i++) {
        shift = 8 * (size - i - 1);

        mkinstr(p, SET, 2, shift);
        mkinstr(p, SR, 0, 0); // Right shift to get byte element value
        mkinstr(p, STR, var->address + i, 0); // put byte value in proper address

        if(shift <= 0) break; // Optimization to prevent unnecessary shifting when shift is zero
        
        mkinstr(p, SET, 2, shift);
        mkinstr(p, SL, 0, 0); // Shift byte element value left to get element actual value
        
        mkinstr(p, MV, 1, 6); // This was the cause of a lot of headache. This line basically prevents a bug in sub_registers
        //where register 1 is assigned and then reassigned during execution specifically if reg_negative = 1
        mkinstr(p, MV, 3, 7); // Uses reserve registers to circumvent sub_registers() bullshit

        addfunc(p, sub_registers(7, 6)); // subtract by the current sum to get leftover total
        mkinstr(p, MV, 1, 3); // Make this the new element actual value
    }
    return p;
}

// Complex functions

VMProgram* var_single_operation(Var* result_var, Var* var1, Var* var2, int operation) {
    VMProgram* p = create_program_array();
    if(!is_same_type(var1, var2) || !is_same_type(result_var, var1)) {
        fprintf(stderr, "Operation failed: variables are not the same type\n");
        return NULL;
    }

    addfunc(p, load_variable(var1, 4));
    addfunc(p, load_variable(var2, 5));
    mkinstr(p, MV, 4, 1);
    mkinstr(p, MV, 5, 2);
    mkinstr(p, operation, 0, 0);
    mkinstr(p, MV, 1, 4);
    addfunc(p, store_variable(result_var, 4));
    return p;
}


VMProgram* add(Var* var_left, Var* var1, Var* var2) {
    return var_single_operation(var_left, var1, var2, 1);
}

VMProgram* subtract(Var* var_left, Var* positive, Var* negative) {
    VMProgram* p = create_program_array();

    addfunc(p, load_variable(positive, 4));
    addfunc(p, load_variable(negative, 5));
    addfunc(p, sub_registers(4, 5));
    mkinstr(p, MV, 1, 4);
    addfunc(p, store_variable(var_left, 4));

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
    VMProgram* p = create_program_array();
    mkinstr(p, HLT, 0, 0);
    return p;
}