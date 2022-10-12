#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
make_instr_func(ret_near)
{
    OPERAND rm;
    
    rm.type=OPR_MEM;
    rm.addr=cpu.gpr[4].val;
    rm.data_size=data_size;
    operand_read(&rm);
    
    cpu.eip=rm.val;
    cpu.gpr[4].val +=data_size/8;
    
    return 1;
}