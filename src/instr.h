#include "programs.h"
#include "vars.h"

enum Opcode{
    NUL,
    ADD,
    FLP,
    OR,
    AND,
    SL,
    SR,
    LT,
    JMP,
    CJP,
    SET,
    MV,
    LD,
    STR,
    HLT,
};

VMProgram* sub_registers(int reg_positive, int reg_negative);

VMProgram* twc1();


VMProgram* execute(Function* f);

VMProgram* set_value(Var* var_left, Var* var_right);

VMProgram* set_value_const(Var* var, int num);

VMProgram* load_variable(Var* var, int reg);

VMProgram* store_variable(Var* var, int reg);

VMProgram* var_single_operation(Var* result_var, Var* var1, Var* var2, int operation);

VMProgram* add(Var* var_left, Var* var1, Var* var2);

VMProgram* subtract(Var* var_left, Var* positive, Var* negative);



VMProgram* halt();



void init_instructions();

void add_instruction(VMProgram* p);

Instruction create_instruction(char opcode, int x, int y);

VMProgram* init_hook(int address);

void set_hook_pointer(unsigned char* ramdisk, int address);