#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near){
    
    
    //push eip;
    OPERAND rm;
    cpu.gpr[4].val-=data_size/8;
    
    rm.type=OPR_MEM;
    rm.addr=cpu.gpr[4].val;
    rm.data_size=data_size;
    
    rm.val=cpu.eip+ 1 + (data_size/8);//important
    //rm.val=cpu.eip;
    
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
    if(data_size==16)
    {
        cpu.eip=cpu.eip&0x0000FFFF;
    }
    
    return 1 + (data_size/8);
}













