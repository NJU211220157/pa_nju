#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	uint32_t base_ = cpu.segReg[sreg].base;
	return offset + base_;
}

// load the invisible part of a segment register

//按字节编址
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	 
	 
	 uint32_t index2 = cpu.segReg[sreg].index;
	 uint32_t addr = (cpu.gdtr.base + index2 * 8);
	 
	 uint32_t base1 = 0, base2 = 0;
	 memcpy(&base1, (void *) (addr + 2), 3);
	 memcpy(&base2, (void *) (addr + 7), 1);
	 cpu.segReg[sreg].base = base1  + (base2 << 24) ;
	 
	 uint32_t limit1 = 0,limit2 = 0;
	 memcpy(&limit1, (void *) addr , 2);
	 memcpy(&limit2, (void *) (addr + 6) , 1);
	 cpu.segReg[sreg].limit = limit1 + ((limit2 & 0xf) << 16);
	 cpu.segReg[sreg].limit = 0xffff;
	 
	 uint32_t type1 = 0;
	 memcpy(&type1, (void *) (addr + 5),1);
	 cpu.segReg[sreg].type = type1 & 0x1f;//取低五位
	 
	 uint32_t privilege_level1 = 0;
	 memcpy(&privilege_level1,(void *)(addr + 5),1);
	 cpu.segReg[sreg].privilege_level = privilege_level1 & 0x60;//只取第6、7位

     uint32_t soft_use1 = 0;
     memcpy(&soft_use1, (void *) (addr + 6), 1);
     cpu.segReg[sreg].soft_use = soft_use1 & 0x10;  //只取第5位
}












