#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/

make_instr_func(inc_rm_v)
{
    OPERAND rm;
    rm.data_size=data_size;
    int len=1;
    len += modrm(eip+1,&rm);//寻址
    operand_read(&rm);
    
    rm.val+=1;
    operand_write(&rm);
    
    return len+1;
}