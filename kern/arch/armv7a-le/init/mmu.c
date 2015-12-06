#include "mmu.h"
#include <drivers/serial/uart.h>

void set_L1_PT_sec(u32 *entry, u32 sec_base, u32 attr) {
	// 0x402:
	// NS 0, nG 0, S 0, AP[2:0] 001, TEX[2:0] 000, Domain 0000
	*entry = (sec_base << 20) | attr;
}

void prepare_first_page_table() {
	uart_spin_puts("MMU: preparing first page table\r\n");
	////
	// Prepare first page table
	////
	u32 *TTB = (u32*)KERN_TTB_BASE_PHY;

	// directly map
	uart_spin_puts("MMU: directly mapping\r\n");
	for (u32 i = 0; i < MEM_SIZE; i++) {
		set_L1_PT_sec(TTB+i, i, KERN_ATTR);
	}
	// device
	uart_spin_puts("MMU: directly mapping device\r\n");
	for (u32 i = DEVICE_MEM_PHY_ST >> 20; i < (DEVICE_MEM_PHY_ED >> 20); i += 0x001) {
		set_L1_PT_sec(TTB+i, i, DEVICE_ATTR);
	}
	// stack
	uart_spin_puts("MMU: mapping kernel stack\r\n");
	for (u32 i = 0; i < KERN_STACK_SIZE; i++) {
		u32 *entry = TTB + ((KERN_STACK - (i << 20)) >> 20);
		u32 *phy = (KERN_STACK_PHY - (i << 20)) >> 20;

#ifdef DEBUG
		uart_spin_puts("mapping ");
		uart_spin_puthex((KERN_STACK - (i << 20)) >> 20);
		uart_spin_puts("to ");
		uart_spin_puthex(phy);
#endif // DEBUG

		set_L1_PT_sec(entry, phy, KERN_ATTR);
	}
	// kern
	uart_spin_puts("MMU: mapping KERN_BASE + phy -> phy\r\n");
	for (u32 i = 0; i < MEM_SIZE; i++) {
		u32 *entry = TTB + ((KERN_BASE + (i << 20)) >> 20);
		u32 *phy = (KERN_BASE_PHY + (i << 20)) >> 20;

#ifdef DEBUG
		uart_spin_puts("mapping ");
		uart_spin_puthex((KERN_BASE + (i << 20)) >> 20);
		uart_spin_puts("to ");
		uart_spin_puthex(phy);
#endif // DEBUG

		set_L1_PT_sec(entry, phy, KERN_ATTR);
	}
}

void setup_TTB() {
	uart_spin_puts("MMU: setting TTB0\r\n");
	////
	// Set TTB0
	////
	asm volatile (
		"MOV R0, %0;"
		"MCR p15, 0, R0, c2, c0, 0;"
		:
		: "r"(KERN_TTB_BASE_PHY)
		: "r0"
	);

}

void setup_domain_permission() {
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
}

void enable_MMU() {
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

	uart_spin_puts("MMU: enabled\r\n");
}
