#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/

static void instr_execute_1op()
{
    operand_read(&opr_src);
    uint32_t temp=cpu.eflags.CF;
    OPREAND t;
    t.val=0x1;
    opr_src.val = alu_add(opr_src.val,t.val,data_size);
    cpu.eflags.CF = temp;
    operand_write(&opr_src);
}


make_instr_impl_1op(inc,rm,v);