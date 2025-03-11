#include "helium.h"

#include "vars.h"

Instruction* execute(Function f);

Instruction* set_value(Var* var_left, Var* var_right);

Instruction* set_value_const(Var* var, unsigned int num);