#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/

make_instr_func(lea)
{
    OPERAND rm,r;
    
    r.data_size=rm.data_size=data_size;
    
    int len=1;
    len += modrm_r_rm(eip+1,&r,&rm);
    
    operand_read(&rm);
    r.val=rm.addr;
    operand_write(&r);
    
    return len;
}