#include "common.h"
#include "memory.h"
#include "memory/memory.h"
#include <memory.h>
#include "string.h"

#include <elf.h>

#ifdef HAS_DEVICE_IDE
#define ELF_OFFSET_IN_DISK 0
#endif

#define STACK_SIZE (1 << 20)

void ide_read(uint8_t *, uint32_t, uint32_t);
void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph, *eph;

#ifdef HAS_DEVICE_IDE
	uint8_t buf[4096];
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
	elf = (void *)buf;
	Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	Log("ELF loading from ram disk.");
#endif

	/* Load each program segment */
	ph = (void *)elf + elf->e_phoff;//初始地址
	eph = ph + elf->e_phnum;//总共有这么多的ph
	for (; ph < eph; ph++)
	{
		if (ph->p_type == PT_LOAD)
		{
/* TODO: copy the segment from the ELF file to its proper memory area */
//如何将数据读取到内存当中？
            Elf32_Addr src_mem_addr=ph->p_vaddr;
            Elf32_Addr src_file_addr=(void *)elf + ph->p_offset;//从此处开始读取目标文件中的数据
            Elf32_Addr dest_file_addr = src_file_addr + 8*ph->p_filesz;
            
            uint32_t count=ph->p_filesz;//总共读这么读写这么多字节
            
            while(src_file_addr<dest_file_addr)
            {
                hw_mem_write(src_mem_addr, 1, (*src_file_addr));
                src_mem_addr++;
                src_file_addr++;
            }
/* TODO: zeror the memory area [vaddr + file_sz, vaddr + mem_sz) */
            hw_mem_write(src_mem_addr, 8*(ph->p_memsz-count), 0);
#ifdef IA32_PAGE
			/* Record the program break for future use */
			extern uint32_t brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if (brk < new_brk)
			{
				brk = new_brk;
			}
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif
	write_cr3(get_ucr3());
#endif
	return entry;
}
