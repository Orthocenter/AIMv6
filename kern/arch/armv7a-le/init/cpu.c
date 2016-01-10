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
// 4. interrupt
// 5. change mode && set up stack (C runtime)
// Final. jump to entry of kernel


#include "mmu.h"
#include "l1cache.h"
#include "except.h"
#include "../zynq7000/init/l2cache.h"
#include "asm/except.h"
#include "asm/mem.h"
#include <drivers/serial/uart.h>

void init_cpu() {
	// 0.
	//  0.1
	//  reason for the order, see L2C-310 3-26
    uart_spin_puts("CPU: invalidating l2 cache\r\n");
    inval_l2cache();
    //  0.2
    uart_spin_puts("CPU: invalidating l1 cache, TLB, branch preditor array\r\n");
    inval_l1cache();
	
	// 1.
	//	1.1
	uart_spin_puts("CPU: preparing first page table\r\n");
	prepare_first_page_table();
	//	1.2
	uart_spin_puts("CPU: setting up TTB\r\n");
	setup_TTB();
	//	1.3
	uart_spin_puts("CPU: enabling MMU\r\n");
	enable_MMU();

	uart_spin_puts("CPU: move pc \r\n");
    asm volatile(
        "MOV R0, %0;"
        "ADD PC, PC, R0;"
        "DSB;"
        "ISB"
        :
        :"r"(KERN_BASE - KERN_BASE_PHY)
    );

    uart_spin_puts("current pc = ");
    u32 pc;
    asm volatile(
        "MOV %0, PC":"=r"(pc)
    );
    uart_spin_puthex(pc);

	uart_spin_puts("CPU: move sp \r\n");
    asm volatile(
        "MOV R0, %0;"
        "ADD SP, SP, R0;"
        "ADD FP, FP, R0;"
        "DSB;"
        "ISB"
        :
        :"r"(KERN_STACK - KERN_STACK_PHY)
    );
    uart_spin_puts("current sp = ");
    u32 sp;
    asm volatile(
        "MOV %0, SP":"=r"(sp)
    );
    uart_spin_puthex(sp);

    uart_spin_puts("CPU: remove direct map\r\n");
    remove_direct_map();

	uart_spin_puts("CPU: invalidating l2 cache\r\n");
	inval_l2cache();
	//  0.2
	uart_spin_puts("CPU: invalidating l1 cache, TLB, branch preditor array\r\n");
	inval_l1cache();

	// 2.
	uart_spin_puts("CPU: enabling l2 cache\r\n");
	enable_l2cache();

	// 3.
	uart_spin_puts("CPU: enabling l1 cache\r\n");
	enable_l1cache();
    
    uart_spin_puts("CPU: Trying to write to ");
    u32 *test = KERN_STACK;
    uart_spin_puthex(333);
    uart_spin_puthex(test-3);
    *(test-3) = *(test-3);
    uart_spin_puts("CPU: Trying to write to ");
    uart_spin_puthex(test-2);
    *(test-2) = *(test-2);
    uart_spin_puts("CPU: Trying to write to ");
    uart_spin_puthex(test-1);
    *(test-1) = *(test-1);
    uart_spin_puts("CPU: Trying to write to ");
    uart_spin_puthex(test);
    *test = *test;
    uart_spin_puts("CPU: mapping test OK\r\n"); 

    // 4.
	uart_spin_puts("CPU: setting up new stack and change to SYS mode\r\n");
    asm volatile (
        "MSR CPSR_c, #0x1F;" // sys mode
        "MOV FP, %0;" // set frame pointer and stack pointer to KERN_STACK_PHY
        "MOV SP, FP;"
        :
        : "r"(KERN_STACK)
        : 
    ); 
	uart_spin_puts("CPU: test new stack ok\r\n");

	// 5. 
	uart_spin_puts("CPU: setting up interrupt handlers\r\n");
    init_handler();

	// final.
	uart_spin_puts("CPU: jumping to entry of kernel\r\n");
	asm volatile (
		"B start_kernel;"
		:
		:
		:
	);

	uart_spin_puts("CPU: this sentence should not be printed\r\n");
}
