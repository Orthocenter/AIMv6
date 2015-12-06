#ifndef _KERN_MM_SLAB_H
#define _KERN_MM_SLAB_H

#include <config.h>
#include "asm/mem.h"

#define SLAB_SIZE 4 // amount of pages by which a cache pool can grow
#define POOL_NUM 10

typedef struct cache_t {
	struct cache_t *next;
} cache_t;


extern const u32 cache_size[POOL_NUM];
extern const u32 cache_shift[POOL_NUM];
cache_t *pools[POOL_NUM];

void init_slabs();

void* alloc_obj(u32 obj_size);
void free_obj(void* addr, u32 obj_size);

#endif //_KERN_MM_SLAB_H
