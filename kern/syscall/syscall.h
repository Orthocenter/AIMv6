#ifndef _KERN_SYSCALL
#define _KERN_SYSCALL

#include <asm/config.h>
#include "kern/sched/proc.h"

enum syscall_type {SBRK = 1, FORK, EXIT, WAIT, SLEEP, PUTS, PUTHEX};

u32 syscall_sbrk(proc_t *proc);
u32 syscall_fork();
u32 syscall_exit();
u32 syscall_wait();
u32 syscall_sleep();
u32 syscall_puts(proc_t *proc);
u32 syscall_puthex(proc_t *proc);

#endif // _KERN_SYSCALL 