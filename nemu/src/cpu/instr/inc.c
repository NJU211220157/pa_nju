#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/

static void instr_execute_1op()
{
    operand_read(&rm);
    rm.val+=1;
    operand_writr(&rm);
}
make_instr_impl_1op(inc,rm,v);