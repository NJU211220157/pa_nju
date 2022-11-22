#include "cpu/cpu.h"
#include "memory/memory.h"

uint32_t hw_mem_read(paddr_t paddr, size_t len);
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
	 SegDesc segdesc;
     segdesc.val[0] = hw_mem_read(cpu.gdtr.base + cpu.segReg[sreg].index * sizeof(SegDesc), 4);
     segdesc.val[1] = hw_mem_read(cpu.gdtr.base + cpu.segReg[sreg].index * sizeof(SegDesc) + 4,4);
	 assert(segdesc.present == 1);
	 
	 cpu.segReg[sreg].base = segdesc.base_15_0 + (segdesc.base_23_16 << 16) + (segdesc.base_31_24 << 24);
	 
	 cpu.segReg[sreg].limit = segdesc.limit_15_0  + (segdesc.limit_19_16 << 16);
	 

}












