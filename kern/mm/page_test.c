#include "page_test.h"
#include <drivers/serial/uart.h>
#include "asm/mem.h"

void page_test() {
	mblock_t *freelist = get_freelist();
	uart_spin_puts("MM/page: start page test\r\n");
	uart_spin_puts("address of freelist: ");
	uart_spin_puthex(&freelist);

	uart_spin_puts("MM/page: first block size: ");
	uart_spin_puthex(freelist->size);
	u32 original_size = freelist->size;

	uart_spin_puts("MM/page: test only one block: ");
	expect(freelist->next, NULL);

	uart_spin_puts("MM/page: test allocate one block\r\n");
	u32 size1 = 10;
	void *addr1 = alloc_pages(size1);
	uart_spin_puts("MM/page: test size of free pages: ");
	expect(freelist->size, original_size - size1);
	uart_spin_puts("MM/page: test address of allocated pages: ");
	expect(addr1, KERN_BASE + FREE_MEM_PHY_ED + 1 - (size1 << PAGE_SHIFT));

	uart_spin_puts("MM/page: test allocate another block\r\n");
	u32 size2 = 33;
	void *addr2 = alloc_pages(size2);
	uart_spin_puts("MM/page: test size of free pages: ");
	expect(freelist->size, original_size - size1 - size2);
	uart_spin_puts("MM/page: test address of allocated pages: ");
	expect(addr2, addr1 - (size2 << PAGE_SHIFT));

	uart_spin_puts("MM/page: test free the first allocated block\r\n");
	free_pages(addr1, size1);
	uart_spin_puts("MM/page: test existence of second free block: ");
	expect(freelist->next != NULL, true); 
	uart_spin_puts("MM/page: test only two free blocks: ");
	expect(freelist->next->next, NULL);
	uart_spin_puts("MM/page: test size of the first free block: ");
	expect(freelist->size, original_size - size2 - size1);
	uart_spin_puts("MM/page: test size of the second free block: ");
	expect(freelist->next->size, size1);
	uart_spin_puts("MM/page: test physical address of the first block: ");
	expect(freelist->addr, (u8*)freelist - KERN_BASE);
	uart_spin_puts("MM/page, test physical address of the second block: ");
	expect(freelist->next->addr, (u8*)freelist->next - KERN_BASE);

	uart_spin_puts("MM/page: test free the second allocated block\r\n");
	free_pages(addr2, size2);
	uart_spin_puts("MM/page: test existence of free blocks: ");
	expect(freelist != NULL, true);
	uart_spin_puts("MM/page: test only one free blocks: ");
	expect(freelist->next, NULL);
	uart_spin_puts("MM/page: test if size of free blocks is same as origin: ");
	expect(freelist->size, original_size);

	uart_spin_puts("MM/page: test allocate aligned pages\r\n");
	u32 size_aligned = 33;
	u32 aligned = 3;
	void *addr_aligned = alloc_aligned_pages(size_aligned, aligned);
	uart_spin_puts("MM/page: test address of allocated pages: ");
	expect((u32)addr_aligned & ((1 << aligned << PAGE_SHIFT) - 1), 0);
	free_pages(addr_aligned, size_aligned);
	uart_spin_puts("MM/page: test only one free blocks after freed: ");
	expect(freelist && (freelist->next == NULL), true);
	uart_spin_puts("MM/page: test if size of free blockes is same as origin: ");
	expect(freelist->size, original_size);

	uart_spin_puts("MM/page: test no enough pages\r\n");
	u32 size_too_many = original_size + 1;
	uart_spin_puts("MM/page: test allocated too many pages: ");
	expect(alloc_pages(size_too_many), NULL);
	uart_spin_puts("MM/page: test if there are any side effects: ");
	expect(freelist && freelist->size == original_size && (freelist->next == NULL), true);

	uart_spin_puts("MM/page: all test passed\r\n");
}
