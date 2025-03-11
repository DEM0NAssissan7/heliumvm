#include "vars.h"

int varsize(Var* var) {
    switch(var->type) {
        case Int:
            return 4;
        case Short:
            return 2;
        case Char:
            return 1;
    }
    return 0;
}

int is_same_type(Var* var1, Var* var2) {
    if(var1->type == var2->type &&
        var1->is_signed == var2->is_signed &&
        ((var1->pointer && var2->pointer) || var1->pointer == var2->pointer)
    ) return 1;
    return 0;
}