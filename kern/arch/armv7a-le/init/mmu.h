#ifndef _KERN_INIT_MMU
#define _KERN_INIT_MMU

#include <config.h>
#include <asm/mem.h>

#define DOMAIN_ACC_PERMISSION (u32)0x00000003
#define DEVICE_ATTR (u32)0x402
#define KERN_ATTR (u32)0x402

void prepare_first_page_table();
void setup_TTB();
void enable_MMU();

#endif /* _KERN_INIT_MMU */
