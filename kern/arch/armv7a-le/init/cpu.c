// Reference: UG585-3.9
// CPU0:
// 0. invalidate for safety
//	0.1 invalidate L2 cache
//	0.2 invalidate L1 caches, TLB, branch predictor array
// 1. MMU
//	1.1 prepare page tables
//	1.2 setup TTB
//	1.3 enable MMU
// 2. L2 cache
// 3. L1 cache
// 4. stack (C runtime) 
// Final. jump to entry of kernel


#include "mmu.h"
#include "l1cache.h"
#include "../zynq7000/init/l2cache.h"

#include <drivers/serial/uart.h>

void init_cpu() {
	// 0.
	//  0.1
	//  reason for the order, see L2C-310 3-26
	uart_spin_puts("CPU: invalidating l2 cache");
	inval_l2cache();
	//  0.2
	uart_spin_puts("CPU: invalidating l1 cache, TLB, branch preditor array");
	inval_l1cache();
	
	// 1.
	//	1.1
	uart_spin_puts("CPU: preparing first page table");
	prepare_first_page_table();
	//	1.2
	uart_spin_puts("CPU: setting up TTB");
	setup_TTB();
	//	1.3
	uart_spin_puts("CPU: enabling MMU");
	enable_MMU();

	// 2.
	uart_spin_puts("CPU: enabling l2 cache");
	enable_l2cache();

	// 3.
	uart_spin_puts("CPU: enabling l1 cache");
	enable_l1cache();

	// 4. 
	uart_spin_puts("CPU: setting up stack");
	asm volatile (
		"MOV FP, %0;" // set frame pointer and stack pointer to KERN_STACK_PHY
		"MOV SP, FP;"
		:
		: "r"(KERN_STACK)
		: 
	);

	// final.
	uart_spin_puts("CPU: jumping to entry of kernel");
	asm volatile (
		"B	 start_kernel;"
		:
		:
		:
	);
}
