#include <drivers/sd/run-elf.h>
#include <drivers/u32/u32.h>
#include <drivers/serial/uart.h>

#define ELF_BASE        0x00200000
#define BUFFER_BASE     0x00200000 // max buffer size 512, will reuse 512 bytes starting from ELF_BASE
#define PH_BASE         0x00200200

void run_elf(u32 elf_lba) {
    volatile u8 *elf_header = (void *)ELF_BASE;
    sd_dma_spin_read(elf_header, 1, elf_lba);

    void (*kernel_entry)() = (void *)load_u32(elf_header + 0x18);

    u32 e_phentsize = elf_header[0x2B] << 8 | elf_header[0x2A];
    u32 e_phnum = elf_header[0x2D] << 8 | elf_header[0x2C];
    u32 e_phoff = load_u32(elf_header + 0x1C);

    // load all program header
    u32 mask8 = 0x1ff; // (1 << 9) - 1
    u32 end_of_ph = e_phoff + e_phnum * e_phentsize;
    // #program header blocks
    //      = #filled blocks + #first block(if any) + #last block(if any)
    //u32 ph_bnum = (e_phnum * e_phentsize >> 9) + (e_phoff & mask8 > 0) + (end_of_ph & mask8 != mask8);
    // approximate value
    u32 ph_bnum = (e_phnum * e_phentsize >> 9) + 2;

    u8 *program_header = PH_BASE;
    sd_dma_spin_read(program_header, ph_bnum, elf_lba + (e_phoff >> 9));

    program_header += e_phoff & mask8;

    u8 *buffer = (void *)BUFFER_BASE;
    for(int i = 0; i < e_phnum; i++) {
        u8 *ph_ent = program_header + e_phentsize * i;
        u32 p_offset = load_u32(ph_ent + 4);
        u8 *p_paddr = (u8 *)load_u32(ph_ent + 12);
        u32 p_filesz = load_u32(ph_ent + 16);
        u32 p_lba = elf_lba + (p_offset >> 9);
        //u32 p_memsz = load_u32(ph_ent + 20);

        // first block
        if(p_offset & mask8 || p_filesz < 512) {
            sd_dma_spin_read(buffer, 1, p_lba);
            p_lba++;
            u8 _end = p_filesz < 512 ? p_filesz : 512;
            for(int j = p_offset & mask8; j < _end; j++) {
                *(p_paddr++) = buffer[j];
            }
        }
        //uart_spin_puthex(p_offset);

        u32 end_of_p = p_offset + p_filesz;
        // filled blocks
        //      lba = elf_lba + #blocks before current program + #first block(if any)
        u32 filled_num = p_filesz - (512 - (p_offset & mask8) & mask8) - (end_of_p & mask8) >> 9;
        filled_num && sd_dma_spin_read(p_paddr, filled_num, p_lba);
        p_paddr += filled_num << 9;
        p_lba += filled_num;

        // last block
        //if(p_filesz > 512 && end_of_p & mask8 != mask8) { //not work
        if(p_filesz > 512 && (end_of_p & mask8) != mask8) {
            sd_dma_spin_read(buffer, 1, p_lba);
            for(int j = 0; j < (end_of_p & mask8); j++) {
                *(p_paddr++) = buffer[j];
            }
        }
    }

    kernel_entry();
}
