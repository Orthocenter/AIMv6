#include "upgtbl.h"

#include <kern/mm/page.h>
#include <kern/mm/slab.h>

// 0x402
	// NS 0, nG 0, S 0, AP[2:0] 001, TEX[2:0] 000, Domain 0000
// 0x422
	// NS 0, nG 0, S 0, AP[2:0] 001, TEX[2:0] 000, Domain 0001
// 0xC02
	// NS 0, nG 0, S 0, AP[2:0] 011, TEX[2:0] 000, Domain 0001

void fuck_set_L1_PT_sec(u32 *entry, u32 sec_base, u32 attr) {
	*entry = (sec_base << 20) | attr;
}

u32* create_page_table() {
	u32 *TTB = alloc_obj(4096); // 4KB per page table
	u32 *KERN_TTB = (u32*)KERN_TTB_BASE;

	for (u32 i = 2048; i < 4096; i++) {
		fuck_set_L1_PT_sec(TTB + i, KERN_TTB[i] >> 20, 0x422);
	}

	fuck_set_L1_PT_sec(TTB + 0x800, (u32)(alloc_pages(1) - KERN_BASE) >> 20, 0xC02);

	fuck_set_L1_PT_sec(TTB, 10, 0xC02); // code

	return TTB;
}

