#include "cpu/instr.h"

void load_sreg(uint8_t sreg);


make_instr_func(jmp_far_imm)
{
    OPERAND rel;
    data_size = 32;
    
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = eip + 1;

    operand_read(&rel);
    uint32_t temp;
    temp = instr_fetch(eip + 5 ,2);
    cpu.segReg[1].val = temp;
    load_sreg(1);
    
    
    int offset = sign_ext(rel.val, rel.data_size);
        // thank Ting Xu from CS'17 for finding this bug
    print_asm_1("jmp", "", 1 + data_size / 8, &rel);

    cpu.eip += offset;

    return 1 + data_size / 8;
}


make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, rel.data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_short)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = 8;//data_size = 8
        rel.addr = eip + 1;

        operand_read(&rel);
        
        
        int offset = sign_ext(rel.val, rel.data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 2, &rel);

        cpu.eip += offset;

        return 2;
}

make_instr_func(jmp_near_indirect)
{
    
        OPERAND rm;
        rm.data_size=data_size;
        rm.sreg = SREG_CS;
        
        int len=1;
        
        len += modrm_rm(eip+1,&rm);
        operand_read(&rm);

        //int offset = sign_ext(rm.val, rm.data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rm);

        cpu.eip = rm.val;

        return 0;
}
























