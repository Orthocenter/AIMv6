#ifndef _KERN_MM_PAGE
#define _KERN_MM_PAGE

#include <config.h>
#include "asm/mem.h"

typedef struct mblock_t{
	struct mblock_t *next;
	void *addr; // physical address
	u32 size;
}mblock_t;

static mblock_t *freelist;

void init_pages();
/* 
 * allocate @num pages, aligned to 2^@aligned page
 */
void* alloc_aligned_pages(u32 num, u32 aligned);
void* alloc_pages(u32 num);

void free_pages(void *addr, u32 num);

#endif // _KERN_MM_PAGE
