#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
static void instr_execute_1op(){
    
    OPERAND rm;
    
    rm.sreg = SREG_CS;
    rm.type=OPR_MEM;
    rm.addr=cpu.gpr[4].val;
    rm.data_size=data_size;
    operand_read(&rm);
    
    opr_src.val=rm.val;
    operand_write(&opr_src);
    
    cpu.gpr[4].val += data_size/8;
}

make_instr_impl_1op(pop,r,v);