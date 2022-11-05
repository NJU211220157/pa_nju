#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"


typedef struct{
    int valid_bit;//有效位
    uint32_t tags;//标记位
    uint8_t data[64];//一个cache block的大小为64B
}CacheLine;

// init the cache
void init_cache();

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data);

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len);

#endif

#endif
