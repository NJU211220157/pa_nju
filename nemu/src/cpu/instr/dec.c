#include "cpu/instr.h"
/*
Put the implementations of `dec' instructions here.
*/

static void instr_execute_1op()
{
    operand_read(&opr_src);
    uint32_t temp=cpu.eflags.CF;
    rm.val=alu_sub(opr_src.val,0x1,data_size);
    cpu.eflags.CF =temp;
    
    operand_write(&opr_src);
}

make_instr_dec(dec,r,v);
make_instr_dec(dec,rm,v);