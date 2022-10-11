#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
int call_near(uint32_t& eip,uint8_t opcode){
    operand imm;
    
    imm.type=OPR_IMM;
    imm.addr=eip+1;
    imm.sreg=SREG_CS;
    imm.data_size=data_size;
    
    operand_read(&imm);
    eip += imm.val;
    
    return len + (data_size/8);
}