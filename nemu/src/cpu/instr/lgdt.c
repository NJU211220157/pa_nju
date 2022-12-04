#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/

make_instr_func(lgdt)
{
    OPERAND rm;
    rm.sreg = SREG_CS;
    rm.data_size = 16;
    int len = 1;
    
    len += modrm_rm(eip + 1,&rm);
    operand_read(&rm);
    cpu.gdtr.limit = rm.val;
    
    rm.addr += 2;
    rm.data_size = 32;
    operand_read(&rm);
    if(data_size == 16){
        cpu.gdtr.base = rm.val &0xffffff;
        return len;
    }
    cpu.gdtr.base = rm.val;
    
    return len;
}
