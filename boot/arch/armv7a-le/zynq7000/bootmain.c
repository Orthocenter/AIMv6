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
#define MBR_BASE		0x00100000

void (*uart_spin_puts)(const char *) = (void *)(PRELOAD_VECTOR_BASE + 0xC);
//void (*uart_spin_puthex)(u32) = (void *)(PRELOAD_VECTOR_BASE + 0x10);
u32 (*load_u32)(u8 *) = (void *)(PRELOAD_VECTOR_BASE + 0x14);
void (*run_elf)(u32) = (void*)(PRELOAD_VECTOR_BASE + 0x18);

void bootloader() {
    uart_spin_puts("MBR: bootloader started\r\n");
    volatile u8 *mbr = (void *)MBR_BASE;
    volatile u8 *partition2ent = mbr + 462;
    u32 elf_lba = load_u32(partition2ent + 0x8);

    run_elf(elf_lba);
}
