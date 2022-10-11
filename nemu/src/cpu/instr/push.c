#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/
static void instr_execute_1op(){
    operand_read(&opr_src);
    cpu.gpr[4].val-=data_size/8;
}
make_instr_impl_1op(push,r,v);
// make_instr_func(push_r_v){
//     uint8_t regIdx=opcode&07;//获取寄存器编号
//     cpu.gpr[4].val-=data_size/8;
//     OPERAND src;
//     src.addr=cpu.gpr[regIdx].val;
//     src.type=OPR_MEM;
//     operand_read(&src);
//     OPERAND dest;
//     dest.addr=cpu.gpr[4].val;
//     dest.type=OPR_MEM;
//     dest.val=src.val;
//     operand_write(&dest);
//     return 1;//指令长度
// }