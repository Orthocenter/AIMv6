#ifndef _KERN_SCHED_CPU
#define _KERN_SCHED_CPU

#include <asm/config.h>
#include "proc.h"
#include "context.h"

#define CPU_NUM 2

typedef struct cpu_t {
	int cpu_id;

	proc_t *proc;

	context_t next_context;
	context_t *scheduler;
} cpu_t;

static cpu_t cpus[CPU_NUM];

cpu_t *get_cpu();

#endif // _KERN_SCHED_CPU