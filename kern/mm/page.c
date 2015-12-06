#include "page.h"

void init_pages() {
	mblock_t *first_block = (mblock_t*)(KERN_BASE + FREE_MEM_PHY_ST);
	freelist = first_block;

	first_block->next = NULL;
	first_block->addr = FREE_MEM_PHY_ST;
	first_block->size = (FREE_MEM_PHY_ED - FREE_MEM_PHY_ST + 1) >> PAGE_SHIFT;
}

void* alloc_pages(u32 num) {
	return alloc_aligned_pages(num, 0);
}

void* alloc_aligned_pages(u32 num, u32 aligned) {
	u32 aligned_shift = aligned + PAGE_SHIFT;
	for(mblock_t *block = freelist, *last_block = NULL; block != NULL; last_block = block, block = block->next) {
		u8 *block_ed = (u8*)block + (block->size << PAGE_SHIFT); // exclusively
		u8 *addr_st = ((u32)(block_ed - (num << PAGE_SHIFT)) >> (aligned_shift)) << (aligned_shift); // aligned start address
		if (addr_st < block) continue; // this block is not big enough, check next one

		u8 *addr_ed = addr_st + (num << PAGE_SHIFT);
		/* case 0:
		 *   -----------
		 *  [  |  | new ]
		 *   -----------
		 */
		if (addr_st > block && addr_ed < block_ed) {
			mblock_t *new_block = (mblock_t*)addr_ed;
			new_block->size = (block_ed - addr_ed) >> PAGE_SHIFT;
			new_block->next = block->next;
			new_block->addr = (u8*)new_block - KERN_BASE;

			block->size = (addr_st - (u8*)block) >> PAGE_SHIFT;
			block->next = new_block;
		} else
		/* case 1:
		 *   -----------
		 *  [|  |  new  ]
		 *   -----------
		 */
		if (addr_st == block && addr_ed < block_ed) {
			mblock_t *new_block = (mblock_t*)addr_ed;
			if (last_block) {
				last_block->next = new_block;
			}
			new_block->size = block->size - num;
			new_block->next = block->next;
			new_block->addr = (u8*)new_block - KERN_BASE;
		} else
		/* case 2:
		 *   -----------
		 *  [     |    |]
		 *   -----------
		 */
		if (addr_st > block && addr_ed == block_ed) {
			block->size -= num;
		} else
		/* case 3:
		 *   -----------
		 *  [|         |]
		 *   -----------
		 */
		if (addr_st == block && addr_ed == block_ed) {
			if(last_block) {
				last_block->next = block->next;
			}
		}

		// todo: initialization?
		return addr_st;
	}

	return NULL;
}

void free_pages(void *addr, u32 num) {
	mblock_t *new_block = (mblock_t*)addr;
	mblock_t *prev_block = NULL;
	if (addr < freelist) {
		new_block->next = freelist;
		new_block->addr = (u8*)new_block - KERN_BASE;
		freelist = new_block;
	} else {
		for(mblock_t *block = freelist; block != NULL; block = block->next) {
			if(addr < block->next || block->next == NULL) {
				new_block->size = num;
				new_block->next = block->next;
				new_block->addr = (u8*)new_block - KERN_BASE;
				block->next = new_block;	
				prev_block = block;
				break;
			}
		}
	}

	if(prev_block && (u8*)prev_block + (prev_block->size << PAGE_SHIFT) == new_block) {
		prev_block->size += new_block->size;
		prev_block->next = new_block->next;
		new_block = prev_block;
	}

	if((u8*)new_block + (new_block->size << PAGE_SHIFT) == new_block->next) {
		new_block->size += new_block->next->size;
		new_block->next = new_block->next->next;
	}
}
