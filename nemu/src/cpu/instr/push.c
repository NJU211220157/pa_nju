#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/
static void instr_execute_1op(){
    operand_read(&opr_src);
    cpu.gpr[4].val-=data_size/8;//esp -= 4
    
    OPERAND rm;
    rm.type=OPR_MEM;
    rm.addr=cpu.gpr[4].val;
    rm.data_size=data_size;
    
    if(opr_src.type==OPR_IMM&&opr_src.data_size==8){
        rm.val=sign_ext(opr_src.val,data_size);
    }
    else{
        rm.val=opr_src.val;
    }
    operand_write(&rm);
    
}
make_instr_impl_1op(push,r,v);
make_instr_impl_1op(push,rm,v);
make_instr_impl_1op(push,i,b);
make_instr_impl_1op(push,i,v);
