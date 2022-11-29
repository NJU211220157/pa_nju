#include "cpu/cpu.h"
#include "memory/memory.h"

uint32_t hw_mem_read(paddr_t paddr, size_t len);

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
    //页目录和页表项的物理页需要偏移20位吗
    //return 0 甚至得到一样的结果
    return 0;
/*	uint32_t dir = laddr >> 22;//只有高十位
	uint32_t page = (laddr << 10 ) >> 22;
	uint32_t offset = laddr & 0xfff;
	
	PDE pde;
    pde.val = hw_mem_read((cpu.cr3.base << 12) + dir * sizeof(PDE), 4);
    assert(pde.present == 1);
    
	PTE pte;
	pte.val = hw_mem_read((pde.page_frame << 12) + page * sizeof(PTE), 4);
	assert(pte.present == 1);
	
	uint32_t p_page = pte.page_frame << 12;
	return p_page + offset;*/
	
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
