#ifndef _KERN_SCHED_CONTEXT
#define _KERN_SCHED_CONTEXT

#include <asm/config.h>

typedef struct context_t{
	u32 cpsr;
	u32 r[16]; // r0 -> r15
	void *ttb;
}context_t;

inline static void copy_context(context_t *dst, context_t *src) {
	dst->cpsr = src->cpsr;
	for(int i = 0; i < 16; i++) dst->r[i] = src->r[i];
	dst->ttb = src->ttb;
}

#endif // _KERN_SCHED_CONTEXT