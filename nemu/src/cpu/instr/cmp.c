#include "cpu/instr.h"
/*
Put the implementations of `cmp' instructions here.
*/

static void instr_execute_2op(){
    operand_read(&opr_src);
    operand_read(&opr_dest);
    opr_src.val=sign_ext(opr_src.val,data_size);
    if(opr_src.type==OPR_IMM&&opr_src.data_size==8&&opr_dest.data_size>8){
       alu_sub(sign_ext(opr_src.val,opr_src.data_size),opr_dest.val,opr_dest.data_size);
    }
    else{
        alu_sub(opr_src.val,opr_dest.val,opr_dest.data_size);
    }
}

make_instr_impl_2op(cmp,rm,r,b);
make_instr_impl_2op(cmp,rm,r,v);
make_instr_impl_2op(cmp,r,rm,b);
make_instr_impl_2op(cmp,r,rm,v);
make_instr_impl_2op(cmp,i,a,b);
make_instr_impl_2op(cmp,i,a,v);
make_instr_impl_2op(cmp,i,rm,b);
make_instr_impl_2op(cmp,i,rm,v);
make_instr_impl_2op(cmp,i,rm,bv);

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



