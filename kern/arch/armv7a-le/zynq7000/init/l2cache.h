#ifndef _KERN_INIT_L2CACHE
#define _KERN_INIT_L2CACHE

#include <config.h>

#define L2CC_BASE_ADDR (void*)0xF8F02000

void inval_l2cache();
void init_interrupts();
void enable_l2cache();

#endif
