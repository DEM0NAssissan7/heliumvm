#include "helium.h"

#ifndef VARS_H
#define VARS_H

enum VarType{
    Int,
    Short,
    Char
};

typedef struct {
    char* name;
    int address; // Address of the variable in memory
    enum VarType type;
    char is_signed;
    int pointer;
} Var;

typedef struct {
    char* name;
    int start_address;
    Var return_var;
    Var* args; // an array of variables
    int num_args;
    VMProgram program;
} Function;

#endif

/* The arguments of a function(arg) are variables that are already assigned in memory
    with the respective types(and stay reserved). This is because helium only has 2 registers,
    therefore, we must use the spaces in memory to store variables. These variables STAY in memory.

    For return_var, we do the exact same thing: we store the result at a fixed place in memory and it
    is a FIXED variable, just like any other variable. We prevent lefthand modification in the compiler itself.
*/

int varsize(Var* var);

int is_same_type(Var* var1, Var* var2);