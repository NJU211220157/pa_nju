#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/

static void instr_execute_1op()
{
    operand_read(&opr_src);
    OPERAND rm;
    rm.type=opr_src.type;
    rm.data_size=opr_src.data_size;
    rm.val=opr_src.val+1;
    rm.addr=opr_src.addr;
    operand_write(&rm);
}
make_instr_impl_1op(inc,rm,v);