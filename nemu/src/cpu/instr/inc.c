#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
    
    //opr_dest.type=opr_src.type;
    opr_dest.addr=opr_src.addr;
    opr_dest.data_size=opr_src.data_size;
    opr_dest.val=opr_src.val + 1;
    
    operand_write(&opr_dest);
}
make_instr_impl_1op(inc,rm,v);

// make_instr_func(inc_rm_v)
// {
//     // OPERAND rm;
//     // rm.data_size=data_size;
//     // int len=1;
//     // len += modrm(eip+1,&rm);//寻址
//     // operand_read(&rm);
    
//     // rm.val+=1;
//     // operand_write(&rm);
    
//     return 3;
// }