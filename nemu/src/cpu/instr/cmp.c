#include "cpu/instr.h"
/*
Put the implementations of `cmp' instructions here.
*/
static void instr_execute_2op(){
    operand_read(&opr_src);
    long res=opr_dest.val-opr_src.val;
    if(res==0){
        cpu.eflags.ZF=0;
    }
    cpu.eflags.SF=res>=0?0:1;
    operand_write(&opr_dest);
}
//make_instr_impl_2op(mov, r, rm, b)

make_instr_impl_2op(cmp,rm,r,b);
make_instr_impl_2op(cmp,rm,r,v);
make_instr_impl_2op(cmp,r,rm,b);
make_instr_impl_2op(cmp,r,rm,v);
make_instr_impl_2op(cmp,i,a,b);
make_instr_impl_2op(cmp,i,a,v);

// make_instr_func(cmp_rm2r_b)
// {
//     OPERAND rm,imm;
//     imm.data_size=rm.data_size=8;
    
//     int len=1;
//     len += modrm_rm(eip+1,&rm);
//     imm.type=OPR_IMM;
//     imm.sreg=SREG_CS;
//     imm.addr=eip+len;
//     operand_read(&opr_src);
//     long res=rm.val-imm.val;
//     if(res==0){
//         cpu.eflags.ZF=0;
//     }
//     cpu.eflags.SF=res>=0?0:1;
//     operand_write(&opr_dest);
    
//     return len+1;
// }



