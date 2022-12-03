#include "cpu/cpu.h"
#include "memory/memory.h"

uint32_t hw_mem_read(paddr_t paddr, size_t len);

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
    //页目录和页表项的物理页需要偏移12位吗
    //return 0 甚至得到一样的结果
	uint32_t dir_ = (laddr >> 22) & 0x3FF;//只有高10位
	uint32_t page_ = ((laddr << 10 ) >> 22) & 0x3FF;//取中间10位
	uint32_t offset_ = laddr & 0xFFF;//低12位
	
	PDE pde;
    pde.val = hw_mem_read( (cpu.cr3.base << 12) + dir_ * sizeof(PDE), sizeof(PDE));
    assert(pde.present == 1);
    
	PTE pte;
	pte.val = hw_mem_read( (pde.page_frame << 12) + page_ * sizeof(PTE), sizeof(PTE));
	assert(pte.present == 1);
	
	uint32_t p_page = pte.page_frame << 12;
	return p_page + offset_;
	
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
