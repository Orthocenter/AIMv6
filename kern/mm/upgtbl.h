#ifndef _KERN_MM_UPGTBL
#define _KERN_MM_UPGTBL

#include <asm/config.h>

u32* create_page_table();

u32* switch_ttb(u32* ttb);

#endif // _KERN_MM_UPGTBL