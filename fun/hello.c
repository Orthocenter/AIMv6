#include <drivers/serial/uart.h>

typedef unsigned int u32;

void set_L1_PT_sec(u32 *entry, u32 sec_base) {
	// 0x400:
	// NS 0, nG 0, S 0, AP[2:0] 001, TEX[2:0] 000, Domain 0000
	*entry = (sec_base << 20) | 0x402;
}

#define TTB_BASE 0x10000000
#define TTBC_USE_TTB0 0x00000000
#define TTBC_USE_TTB1 0x80000001
#define DOMAIN_ACC_PERMISSION 0x00000003

void sayhello() {
	uart_spin_puts("17 Hello world from partition2!\r\n");

	uart_spin_puts("MMU: preparing first page table\r\n");
	////
	// Prepare first page table
	////
	u32 *TTB = (u32*)TTB_BASE;
	for (u32 i = 0; i < 4096; i++) {
		if (i % 256 == 0) uart_spin_puthex(i);
		set_L1_PT_sec(TTB++, i);
	}

	uart_spin_puts("MMU: setting TLB control to use TTB1\r\n");
	////
	// Set TTB Control to use TTB1
	//// Ref
	// 1138 B4.9.3
	////
	asm volatile (
		"MOV R0, %0;"
		"MCR p15, 0, R0, c2, c0, 2;"
		:
		: "r"(TTBC_USE_TTB0)
		: "r0"
	);

	uart_spin_puts("MMU: setting TTB1\r\n");
	////
	// Set TTB1
	////
	asm volatile (
		"MOV R0, %0;"
		"MCR p15, 0, R0, c2, c0, 0;"
		:
		: "r"(TTB_BASE)
		: "r0"
	);

	uart_spin_puts("MMU: grant access permission to domain 0\r\n");
	////
	// Grant access permission to domain 0
	//// Ref
	// 1138 B4.3.2
	////
	asm volatile (
		"MOV R0, %0;"
		"MCR p15, 0, R0, c3, c0, 0;"
		:
		: "r"(DOMAIN_ACC_PERMISSION)
		: "r0"
	);
	
	uart_spin_puts("MMU: enabling MMU\r\n");
	////
	// Enable MMU
	//// Reference
	// MCR:	1138 A4.1.32
	// MRC: 1138 A4.1.36
	// ORR: 1138 A4.1.42, 2736 A8.8.122
	// CP15: 1138 B4.9
	// CP15R1: 1138 B4.9.2
	////
	asm volatile (
		"MRC p15, 0, R0, c1, c0, 0;"
		"ORR R0, R0, #0x1;"
		"MCR p15, 0, R0, c1, c0, 0;"
		:
		:
		: "r0"
	);

	uart_spin_puts("MMU enabled\r\n");
}
