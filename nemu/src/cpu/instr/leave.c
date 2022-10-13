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
    
    
    OPERAND rm;
    
    rm.type=OPR_MEM;
    rm.addr=cpu.gpr[4].val;
    rm.data_size=data_size;
    operand_read(&rm);
    
    rbp.val=rm.val;
    operand_write(&rbp);
    
    cpu.gpr[4].val += data_size/8;
}