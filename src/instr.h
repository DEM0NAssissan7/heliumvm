#include "helium.h"

#include "vars.h"

VMProgram* execute(Function* f);

VMProgram* set_value(Var* var_left, Var* var_right);

VMProgram* set_value_const(Var* var, unsigned int num);

VMProgram* load_variable(Var* var, int reg);

VMProgram* store_variable(Var* var, int reg);

VMProgram* var_single_operation(Var* result_var, Var* var1, Var* var2, int operation);

VMProgram* add(Var* var_left, Var* var1, Var* var2);

VMProgram* subtract(Var* var_left, Var* positive, Var* negative);



VMProgram* halt();



void init_instructions();

void add_instruction(VMProgram* p);

VMProgram* init_hook(int address);

void set_hook_pointer(unsigned char* ramdisk, int address);