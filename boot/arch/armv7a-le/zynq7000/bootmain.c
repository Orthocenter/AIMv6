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
#define ELF_BASE		0x00200000
#define BUFFER_BASE		0x00200000 // max buffer size 512, will reuse 512 bytes starting from ELF_BASE

//void (*uart_spin_puts)(const char *) = (void *)(PRELOAD_VECTOR_BASE + 0xC);
//void (*uart_spin_puthex)(u32) = (void *)(PRELOAD_VECTOR_BASE + 0x10);
u32 (*load_u32)(u8 *) = (void *)(PRELOAD_VECTOR_BASE + 0x14);
int (*sd_dma_spin_read)(u32 pa, u16 count, u32 offset) = (void *)(PRELOAD_VECTOR_BASE + 0x18);

void bootloader() {
    //uart_spin_puts("b\r\n");
    volatile u8 *mbr = (void *)MBR_BASE;
    //volatile u8 *partition2ent = mbr + 462;
    //u32 elf_lba = load_u32(partition2ent + 0x8);
    u32 elf_lba = load_u32(mbr + 0x1d6);

    volatile u8 *elf_header = (void *)ELF_BASE;
    sd_dma_spin_read(elf_header, 1, elf_lba);

    void (*kernel_entry)() = (void *)load_u32(elf_header + 0x18);

    u32 e_phentsize = elf_header[0x2B] << 8 | elf_header[0x2A];
    u32 e_phnum = elf_header[0x2D] << 8 | elf_header[0x2C];
    u32 e_phoff = load_u32(elf_header + 0x1C);

    // load all program header
    u32 mask8 = (1 << 9) - 1;
    u32 end_of_ph = e_phoff + e_phnum * e_phentsize;
    // #program header blocks
    // 		= #filled blocks + #first block(if any) + #last block(if any)
    //u32 ph_bnum = (e_phnum * e_phentsize >> 9) + (e_phoff & mask8 > 0) + (end_of_ph & mask8 != mask8);
    // approximate value
    u32 ph_bnum = (e_phnum * e_phentsize >> 9) + 2;

    u8 *program_header = elf_header + 512;
    sd_dma_spin_read(program_header, ph_bnum, elf_lba + (e_phoff >> 9));

    program_header += e_phoff & mask8; 

    for(int i = 0; i < e_phnum; i++) {
        u8 *ph_ent = program_header + e_phentsize * i;
        u32 p_offset = load_u32(ph_ent + 4);
        u8 *p_paddr = (u8 *)load_u32(ph_ent + 12);
        u32 p_filesz = load_u32(ph_ent + 16);
        u32 p_lba = elf_lba + (p_offset >> 9);
        //u32 p_memsz = load_u32(ph_ent + 20);

        // first block
        if(p_offset & mask8) {
            u8 *buffer = (void *)BUFFER_BASE;
            sd_dma_spin_read(buffer, 1, p_lba);
            p_lba += 1;
            for(int j = p_offset & mask8; j < 512; j++) {
                *(p_paddr++) = buffer[j];
            }
        }
        //uart_spin_puthex(p_lba);

        u32 end_of_p = p_offset + p_filesz;
        // filled blocks
        // 		lba = elf_lba + #blocks before current program + #first block(if any)
        u32 filled_num = p_filesz - (512 - (p_offset & mask8) & mask8) - (end_of_p & mask8) >> 9;
        if(filled_num) {
            sd_dma_spin_read(p_paddr, filled_num, p_lba);
        }
        p_paddr += filled_num << 9;
        p_lba += filled_num;
        //uart_spin_puthex(p_lba);

        // last block
        if(p_filesz > 512 && end_of_p & mask8 != mask8) {
            u8 *buffer = (void *)BUFFER_BASE;
            sd_dma_spin_read(buffer, 1, p_lba);
            for(int j = 0; j < end_of_p & mask8; j++) {
                *(p_paddr++) = buffer[j];
            }
        }
    }

    kernel_entry();
}
