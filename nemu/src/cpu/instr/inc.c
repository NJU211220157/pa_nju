#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
    operand_read(&opr_dest);
    opr_dest.data_size=data_size;
    opr_dest.val=1;
    opr_dest.type=opr_src.type;
    
    uint32_t temp=cpu.eflags.CF;
    
    opr_src.val=alu_add(opr_src.val,opr_dest.val,data_size);
    
    cpu.eflags.CF=temp;
    
    operand_write(&opr_src);
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