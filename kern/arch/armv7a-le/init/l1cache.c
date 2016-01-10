#include "l1cache.h"

void inval_l1cache() {
	asm volatile (
		"MOV R0, #0;"
		"MCR P15, 0, R0, C8, C7, 0;" // invalidate TLBs
		"MCR P15, 0, R0, C7, C5, 0;" // invalidate I-cache
		"MCR P15, 0, R0, C7, C5, 6;" // invalidate branch predictor array
		"MCR P15, 0, R11, C7, C14, 2;" // invalidate D-cache
		"DSB;" // ensure completion of the l1 inval
        "ISB;"
		:
		:
		:
	);
}

void enable_l1cache() {
	asm volatile (
		"MRC p15, 0, r0, c1, c0, 0;"
		"ORR r0, #0x1000;"
		"ORR r0, #0x0004;"
		"MCR P15, 0, R0, C1, C0, 0;" // enable I-cache, D-cache
		"DSB;" // allow MMU to start
		"ISB;" // flush pre-fetch buffer
		:
		:
		:
	);
}
