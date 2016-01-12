#ifndef _KERN_INIT_MMU
#define _KERN_INIT_MMU

#include <config.h>
#include <asm/mem.h>

#define DOMAIN_ACC_PERMISSION (u32)0x00000007 // D1 01(check permission) for user, D0 11(manager) for kern
// 0x402:
	// NS 0, nG 0, S 0, AP[2:0] 001, TEX[2:0] 000, Domain 0000
#define DEVICE_ATTR (u32)0x402
#define KERN_ATTR (u32)0x402

void prepare_first_page_table();
void remove_direct_map();
void setup_TTB();
void enable_MMU();

#endif /* _KERN_INIT_MMU */
