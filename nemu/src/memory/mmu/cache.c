#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include <stdlib.h>
CacheLine cache[128][8];//总共有128组,1024个cache行,每组有8行


// init the cache
void init_cache()
{
	for(int i=0;i<128;i++){
	    for(int j=0;j<8;j++)
	        cache[i][j].valid_bit = 0;
	}
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
    uint32_t block_offset = paddr % 64;//低6位
	uint32_t set_index = (paddr >> 6) % 128;
	uint32_t tag_bits = paddr >> 13; //6+7
    bool across = 0;
	if(block_offset + len > 64){//跨行了，需要分开访问cache行
	    across = 1;
	}
	memcpy(hw_mem + paddr, &data, len);//write through
	for(int i=0; i<8; i++){
	    if(cache[set_index][i].valid_bit == 1 && cache[set_index][i].tags == tag_bits){
	        if(!across){
	            memcpy(cache[set_index][i].data + block_offset, &data, len);
	        }
	        else{
	            memcpy(cache[set_index][i].data + block_offset, &data, 64 - block_offset);
	            set_index = (set_index + (i + 1)/8) % 128;   i = (i + 1) % 8;
	            tag_bits = (paddr + 64 - block_offset) >> 13;
	            cache[set_index][i].valid_bit = 1;//更新组号和行号后设置标志位
	            cache[set_index][i].tags = tag_bits;
	            memcpy(cache[set_index][i].data, &data, block_offset + len - 64);
	        }
	    }
	}
}

//是否需要处理小端方式带来的影响？

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
    uint32_t res = 0;bool found = 0;
    uint32_t block_offset = paddr % 64;//低6位
	uint32_t set_index = (paddr >> 6) % 128;
	uint32_t tag_bits = paddr >> 13; //6+7
    bool across = 0;
	if(block_offset + len > 64){//跨行了，需要分开访问cache行
	    across = 1;
	}
	for(int i=0;i<8;i++){
	    if(cache[set_index][i].valid_bit == 1 && cache[set_index][i].tags == tag_bits){
	        if(!across)
	            memcpy(&res, cache[set_index][i].data + block_offset, len);
	        else{
	            memcpy(&res, cache[set_index][i].data+block_offset, 64 - block_offset);
	            set_index = (set_index + (i + 1)/8) % 128;   i = (i + 1) % 8;
	            memcpy(&res , cache[set_index][i].data, len + block_offset - 64);
	        }
	        found = 1;
	    }
	}
	if(!found){
	    for(int i=0;i<8;i++){
	        if(cache[set_index][i].valid_bit == 0){
	            cache[set_index][i].valid_bit = 1;
	            cache[set_index][i].tags = tag_bits;
	            if(!across)
	                memcpy(cache[set_index][i].data + block_offset,hw_mem + paddr,len);
	            else{
	                memcpy(cache[set_index][i].data + block_offset,hw_mem + paddr,64 - block_offset);
	                set_index = (set_index + (i + 1)/8) % 128;   i = (i + 1) % 8;
	                tag_bits = (paddr + 64 - block_offset) >> 13;
	                cache[set_index][i].valid_bit = 1;//更新组号和行号后设置标志位
	                cache[set_index][i].tags = tag_bits;
	                memcpy(cache[set_index][i].data,hw_mem + paddr + 64 - block_offset,len + block_offset - 64);
	            }
	            return cache_read(paddr,len);
	        }
	    }
	    int i = rand() % 8;//随机一个cache行
    	if(!across)
            memcpy(cache[set_index][i].data + block_offset,hw_mem + paddr,len);
    	else{
    	    memcpy(cache[set_index][i].data + block_offset,hw_mem + paddr,64 - block_offset);
    	    set_index = (set_index + (i + 1)/8) % 128;   i = (i + 1) % 8;
    	    tag_bits = (paddr + 64 - block_offset) >> 13;
    	    cache[set_index][i].valid_bit = 1;
	        cache[set_index][i].tags = tag_bits;
    	    memcpy(cache[set_index][i].data, hw_mem + paddr + 64 - block_offset ,len + block_offset - 64);
    	}
    	 return cache_read(paddr,len);
	}
	return res;
}

