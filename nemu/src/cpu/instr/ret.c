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
    
    if(data_size==16)
    {
        cpu.eip=cpu.eip&0x0000FFFF;
    }
    
    cpu.gpr[4].val += data_size/8;
    
    return 0;
}

make_instr_func(ret_near_imm16)
{
    OPERAND rm;
    
    rm.type=OPR_MEM;
    rm.addr=cpu.gpr[4].val;
    rm.data_size=data_size;
    operand_read(&rm);
    
    cpu.eip=rm.val;
    if(data_size==16)
    {
        cpu.eip=cpu.eip&0x0000FFFF;
    }
    cpu.gpr[4].val += data_size/8;
    
    OPERAND imm;
    imm.type=OPR_IMM;
    imm.type=SREG_CS;
    imm.addr=eip+1;
    imm.data_size=16;
    operand_read(&imm);
    cpu.gpr[4].val += imm.val;
    
    return 0;
}




















