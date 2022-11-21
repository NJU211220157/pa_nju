#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/

make_instr_func(lgdt)
{
    uint32_t rm1,rm2;
    uint32_t addr = cpu.eip + 2;
    
    memcpy(&rm1,(void*)addr,2);
    memcpy(&rm2,(void*)(addr + 2),4);
    cpu.gdtr.limit = rm1;
    cpu.gdtr.base = rm2;
    
    return 6;
}
