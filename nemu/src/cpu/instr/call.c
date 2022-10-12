#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near){
    
    OPERAND rm;
    
    rm.type=OPR_MEM;
    rm.addr=cpu.gpr[4].val;
    rm.data_size=data_size;
    rm.val=cpu.eip;
    
    operand_write(&rm);
    
    OPERAND rel;
    rel.type=OPR_IMM;
    rel.addr=eip+1;
    rel.sreg=SREG_CS;
    rel.data_size=data_size;
    
    operand_read(&rel);
    int offset = sign_ext(rel.val, data_size);
    print_asm_1("jmp", "", 1 + data_size / 8, &rel);

    cpu.eip += offset;
    
    return 1 + (data_size/8);
}