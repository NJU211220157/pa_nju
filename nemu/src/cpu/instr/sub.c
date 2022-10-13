#include "cpu/instr.h"
/*
Put the implementations of `sub' instructions here.
*/

static void instr_execute_2op()
{
    operand_read(&opr_src);
    operand_read(&opr_dest);
    opr_dest.val=alu_sub(opr_src.val,opr_dest.val,data_size);
    operand_write(&opr_dest);
}

make_instr_impl_2op(sub,i,rm,b);
make_instr_impl_2op(sub,i,rm,v);
make_instr_impl_2op(sub,i,rm,bv);
make_instr_impl_2op(sub,i,a,b);
make_instr_impl_2op(sub,i,a,v);
make_instr_impl_2op(sub,r,rm,b);
make_instr_impl_2op(sub,r,rm,v);
make_instr_impl_2op(sub,rm,r,b);
make_instr_impl_2op(sub,rm,r,v);