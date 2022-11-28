#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "memory/mmu/cache.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
#ifdef CACHE_ENABLED
    ret = cache_read(paddr,len);
#else
	ret = hw_mem_read(paddr, len);
#endif
	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
#ifdef CACHE_ENABLED
    cache_write(paddr, len, data);
#else
	hw_mem_write(paddr, len, data);
#endif
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{
    if(cpu.cr0.pg == 1){
        if((laddr % 0x1000 + len) > 0x1000){
            uint32_t paddr_1 = page_translate(laddr);
            uint32_t len1 = 0x1000 - laddr % 0x1000;
            uint32_t val_1 = paddr_read(paddr_1, len1);
            
            uint32_t paddr_2 = page_translate(laddr + len1);
            uint32_t len2 = len - len1;
            uint32_t val_2 = paddr_read(paddr_2, len2);
            
            return (val_2 << len1 * 8) + val_1;
        }
        else{
            uint32_t paddr = page_translate(laddr);
            return paddr_read(paddr, len);
        }
    }
    else
	    return paddr_read(laddr, len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
	if(cpu.cr0.pg == 1){
        if((laddr % 0x1000 + len) > 0x1000){
            uint32_t paddr_1 = page_translate(laddr);
            uint32_t len1 = 0x1000 - laddr % 0x1000;
            uint32_t data_1 = data & (0xffff >> (32 - len1 * 8));//低8位
            paddr_write(paddr_1, len1 ,data_1);
            
            uint32_t paddr_2 = page_translate(laddr + len1);
            uint32_t len2 = len - len1;
            uint32_t data_2 = data >> (len1 * 8);
            paddr_write(paddr_2, len2, data_2);
        }
        else{
            uint32_t paddr = page_translate(laddr);
            paddr_write(paddr, len ,data);
        }
    }
    else
	    paddr_write(laddr, len , data);
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	return laddr_read(vaddr, len);
#else
    uint32_t laddr = vaddr;
    if(cpu.cr0.pe == 1){
        laddr = segment_translate(vaddr, sreg);
    }
    return laddr_read(laddr, len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	laddr_write(vaddr, len, data);
#else
    uint32_t laddr = vaddr;
    if(cpu.cr0.pe == 1){
        laddr = segment_translate(vaddr, sreg);
    }
    laddr_write(laddr, len, data);
#endif
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
#ifdef CACHE_ENABLED
    init_cache();
#endif
#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
