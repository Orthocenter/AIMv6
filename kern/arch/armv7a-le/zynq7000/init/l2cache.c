#include "l2cache.h"

// References:
//	- L2C-310
//	- UG585: 3.4
//	- https://lists.rtems.org/pipermail/devel/2014-March/005766.html

void inval_l2cache() {
	// -1.
	u32 *L2CC_ENABLE = L2CC_BASE_ADDR + 0x100;
	*L2CC_ENABLE = 0;

	// 0.
	u32 *unknown_r = 0xF8000A1C; // unknown system level control register, UG585 1585
	*unknown_r = 0x020202; // UG585 102

	// 1.
	// set latencies of RAM accesses
	u32 *L2CC_TAG_RAM = L2CC_BASE_ADDR + 0x108;
	u32 *L2CC_DATA_RAM = L2CC_BASE_ADDR + 0x10C;
	*L2CC_TAG_RAM |= 0b000100010001; // set the latencies for the 3 tag RAM attributes to 2
	*L2CC_DATA_RAM |= 0b000100100001; // set the write access and setup latencies of the data RAM to 2
									  // and set the read access latency of the data RAM to 3
	// 2.
	u32 *L2CC_INV_WAY = L2CC_BASE_ADDR + 0x77C;
	*(L2CC_INV_WAY) = 0xFFFF; // invalidate all entries l2 cache

	u32 *L2CC_CACHE_SYNC = L2CC_BASE_ADDR + 0x730;
	while(*L2CC_CACHE_SYNC); // ensure completion of the L2 inval
}

void init_interrupts() {}

void enable_l2cache() {
	u32 *L2CC_ENABLE = L2CC_BASE_ADDR + 0x100;
	*L2CC_ENABLE = 1;
}
