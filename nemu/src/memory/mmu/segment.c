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
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	 uint32_t index = cpu.segReg[sreg].index;
	 uint32_t addr = (cpu.gdtr.base + index * 64);
	 
	 uint32_t base1, base2;
	 memcpy(&base1,(void *) (addr + 16),3);
	 memcpy(&base2,(void *) (addr + 56),1);
	 cpu.segReg[sreg].base = base1  + (base2<<24) ;
	 
	 uint32_t limit1,limit2;
	 memcpy(&limit1,(void *) addr , 2);
	 memcpy(&limit2,(void *) (addr + 48) , 1);
	 cpu.segReg[sreg].limit = limit1 + ((limit2<<28)>>12);
	 
	 uint32_t type1;
	 memcpy(&type1,(void *)(addr + 40),1);
	 cpu.segReg[sreg].type = type1 & 0xf;

}












