#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/

make_instr_func(leave)
{
    OPERAND rbp,rsp;
    
    rbp.type=OPR_REG;
    rbp.addr=0x101;
    rbp.data_size=data_size;
    operand_read(&rbp);
    
    rsp.type=OPR_REG;
    rsp.addr=0x100;
    rsp.data_size=data_size;
    rsp.val=rbp.val;
    operand_write(&rsp);
    
    cpu.gpr[4].val += data_size/8;
    
    return 1;
}