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
    len += modrm_r_rm(eip+1,&r,&rm);//读取 r和rm的地址
    
    if(r.data_size==16){
        r.val=rm.addr&0xFFFF;
    }
    else{
        r.val=rm.addr;
    }
    
    
    print_asm_2("lea", "", 1 + 2*(data_size / 8), &r,&rm);
    
    operand_write(&r);
    
    return len;
}