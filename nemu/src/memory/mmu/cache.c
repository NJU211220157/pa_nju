#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include <stdlib.h>

CacheLine cache[128][8];//总共有128组,1024个cache行,每组有8行

uint32_t hw_mem_read(paddr_t paddr, size_t len);
void hw_mem_write(paddr_t paddr, size_t len, uint32_t data);
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
	memcpy(hw_mem + paddr, &data, len);//write through and not write allocate
	for(int i=0; i<8; i++){
	    if(cache[set_index][i].valid_bit == 1 && cache[set_index][i].tags == tag_bits){
	        if(!across){
	            memcpy(cache[set_index][i].data + block_offset, &data, len);
	        }
	        else{
	            memcpy(cache[set_index][i].data + block_offset, &data , 64 - block_offset);
	            cache_write(paddr + 64 - block_offset, len + block_offset - 64 , data>> ((64 - block_offset)*4) );
	        }
	    }
	}
}

//是否需要处理小端方式带来的影响？

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
    uint32_t res = 0;bool found = 0;
    uint32_t block_offset = paddr & 0x3F;//低6位
	uint32_t set_index = (paddr >> 6) & 0x7F;//中间的七位
	uint32_t tag_bits = paddr >> 13; //6+7
    bool across = 0;
	if(block_offset + len > 64){//跨行了，需要分开访问cache行
	    across = 1;
	}
	for(int i = 0; i < 8; i++){
	    if(cache[set_index][i].valid_bit == 1 && cache[set_index][i].tags == tag_bits){
	        if(!across)
	            memcpy(&res, cache[set_index][i].data + block_offset, len);
	        else{
	            //第二行不一定在cache中
	            union{
	                uint32_t data;
	                char byte[4];
	            }result;
	            memset(&result,0,4);
	            memcpy(&result, cache[set_index][i].data + block_offset, 64 - block_offset);
	            paddr += 64 - block_offset;//更新paddr
	            uint32_t sec_res = cache_read(paddr , len + block_offset - 64);
	            memcpy(result.byte + 64 - block_offset, &sec_res, len + block_offset - 64);
	            return result.data;
	        }
	        found = 1;
	    }
	}
	if(!found){
	    for(int i = 0;i<8;i++){
	        if(cache[set_index][i].valid_bit == 0){ //存在空闲行
	            cache[set_index][i].valid_bit = 1;
	            cache[set_index][i].tags = tag_bits;
	            if(!across)
	                memcpy(cache[set_index][i].data ,hw_mem + paddr - block_offset , 64);
	            else{
	                union{
            	        uint32_t data;
            	        char byte[4];
	                }result;
	                memcpy(cache[set_index][i].data , hw_mem + paddr - block_offset , 64 );//将整行都写进cache里面
        	        memcpy(&result, cache[set_index][i].data + block_offset, 64 - block_offset);
        	        uint32_t sec_res = cache_read(paddr + 64 - block_offset, len + block_offset - 64 );//读取第二块的内容
        	        memcpy(result.byte + 64 - block_offset , &sec_res , len + block_offset - 64);
        	        return result.data;
	            }
	            memcpy(&res , hw_mem + paddr, len);
	            return res;
	        }
	    }
	    int i = rand() % 8;
    	if(!across){
    	    cache[set_index][i].valid_bit = 1;
	        cache[set_index][i].tags = tag_bits;
            memcpy(cache[set_index][i].data ,hw_mem + paddr - block_offset , 64);
            memcpy(&res , cache[set_index][i].data + block_offset, len);
	        return res;
    	}
    	else{
    	    union{
    	        uint32_t data;
    	        char byte[4];
	        }result;
    	    cache[set_index][i].valid_bit = 1;
	        cache[set_index][i].tags = tag_bits;
    	    memcpy(cache[set_index][i].data ,hw_mem + paddr - block_offset , 64);//需要读整行
	        memcpy(&result, cache[set_index][i].data + block_offset, 64 - block_offset);
	        uint32_t sec_res = cache_read(paddr + 64 - block_offset, len + block_offset - 64 );
	        memcpy(result.byte + 64 - block_offset , &sec_res , len + block_offset - 64);
	        return result.data;
    	}
	}
	return res;
}

