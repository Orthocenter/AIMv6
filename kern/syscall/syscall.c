#include "syscall.h"
#include <drivers/serial/uart.h>
#include "kern/sched/proc.h"
#include "kern/sched/sched_cpu.h"

u32 syscall_sbrk(proc_t *proc) {
	u32 size = proc->context->r[1];
	return 1;
}

u32 syscall_puts(proc_t *proc) {
	uart_spin_puts(proc->context->r[1]);
	return 0;
}

u32 syscall_puthex(proc_t *proc) {
	uart_spin_puthex(proc->context->r[1]);
	return 0;
}