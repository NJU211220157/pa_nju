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
	 SegDesc* segDesc = (void *)(cpu.gdtr.base + index * 8);//段表首地址
	 uint32_t base1, base2, base3;
	 base1 = segDesc->base_15_0;
	 base2 = segDesc->base_23_16 << 16;
	 base3 = segDesc->base_31_24 << 24;
	 cpu.segReg[sreg].base = base1 + base2 + base3 ;
	 cpu.segReg[sreg].limit = segDesc->limit_15_0 + (segDesc->limit_19_16 << 16);
	 cpu.segReg[sreg].type = segDesc->type;
	 cpu.segReg[sreg].privilege_level = segDesc->privilege_level;
	 cpu.segReg[sreg].soft_use = segDesc->soft_use;
}
