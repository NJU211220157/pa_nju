#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/

//如何区分不同的data_size?

make_instr_func(lea)
{
    OPERAND rm,r;
    
    r.data_size=rm.data_size=data_size;
    
    int len=1;
    len += modrm_r_rm(eip+1,&r,&rm);
    
    r.val=rm.addr;
    
    operand_write(&r);
    
    return len;
}