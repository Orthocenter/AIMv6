#ifndef _KERN_SCHED_PROC
#define _KERN_SCHED_PROC

#include <asm/config.h>
#include "context.h"

#define MAX_PROC_NUM 128
#define DEFAULT_PROC_CPSR 0b0000010000
#define DEFAULT_PROC_USER_STACK 0x000FFFFC

enum proc_state {UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE};

typedef struct proc_t{
	u32 size;					// Size of heap, in Page

	void *ttb;					// Page table
	u8 *kstack;					// Bottom of kernel stack for this process
	enum proc_state state;		// Process state

	volatile int pid;			// Process ID
	struct proc_t *parent;		// Parent process

	context_t *context;			// Context, swtch() here to run process

	int killed;					// If non-zero, have been killed
}proc_t;

static u32 cpid = 0;

static proc_t procs[MAX_PROC_NUM];

void init_procs();

proc_t *get_procs();

proc_t *create_proc();

#endif // _KERN_SCHED_PROC