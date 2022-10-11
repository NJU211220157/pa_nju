#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near){
    OPERAND rel;
    imm.type=OPR_IMM;
    imm.addr=eip+1;
    imm.sreg=SREG_CS;
    imm.data_size=data_size;
    
    operand_read(&imm);
    int offset = sign_ext(rel.val, data_size);
    print_asm_1("jmp", "", 1 + data_size / 8, &rel);

    cpu.eip += offset;
    
    return 1 + (data_size/8);
}