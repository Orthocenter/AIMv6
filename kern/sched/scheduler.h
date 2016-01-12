#ifndef _KERN_SCHED_SCHEDULER
#define _KERN_SCHED_SCHEDULER

#include "context.h"

void switch_to(const context_t *ctx);
static void start_scheduler();
void* get_start_scheduler();

#endif // _KERN_SCHED_SCHEDULER