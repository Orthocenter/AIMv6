/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>

#define PRELOAD_VECTOR_BASE	0x1ff00000

void (*uart_init)(void) = (void *)(PRELOAD_VECTOR_BASE + 0x4);
void (*uart_enable)(void) = (void *)(PRELOAD_VECTOR_BASE + 0x8);
void (*uart_spin_puts)(const char *) = (void *)(PRELOAD_VECTOR_BASE + 0xC);
void (*uart_spin_puthex)(u32) = (void *)(PRELOAD_VECTOR_BASE + 0x10);
void (*uart_spin_puthex_ptr)(u8 *) = (void *)(PRELOAD_VECTOR_BASE + 0X14);

void bootloader() {
	uart_spin_puts("bootloader started! \r\n");
        volatile u8 *mbr = (void *)0x100000;
        volatile u8 *partition2_entry = mbr + 462;
        u8 *elf_lba = partition2_entry + 0x8;
        u8 *partition2_length = partition2_entry + 0xC;

#ifdef DEBUG
	uart_spin_puts("address of elf_lba: ");
	uart_spin_puthex(elf_lba);
	uart_spin_puts("\r\n");

        uart_spin_puts("lba of elf: ");
	uart_spin_puthex_ptr(elf_lba);
	uart_spin_puts("\r\n");

	uart_spin_puts("address of partition2_length: ");
	uart_spin_puthex(partition2_length);
	uart_spin_puts("\r\n");

        uart_spin_puts("length of partition2: ");
	uart_spin_puthex_ptr(partition2_length);
	uart_spin_puts("\r\n");
#endif
}

