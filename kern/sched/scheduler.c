#include "scheduler.h"
#include "proc.h"
#include "context.h"
#include <asm/mem.h>
#include "sched_cpu.h"

void switch_to(const context_t *ctx) {
    cpu_t *cpu = get_cpu();
    copy_context(&(cpu->next_context), ctx);

	// change ttb
	asm volatile(
		"MOV R0, %0;"
		"MCR p15, 0, R0, c2, c0, 0;" // change ttb
		"MOV R0, #0;"
		"MCR P15, 0, R0, C8, C7, 0;" // invalidate TLBs
		"DSB;"
		"ISB"
		:
		:"r"(cpu->next_context.ttb - KERN_BASE)
		:"r0"
	);

	// load saved CPSR to SPSR
	asm volatile(
		"MSR SPSR, %0"
		:
		:"r"(cpu->next_context.cpsr)
		:
	);

	// restore R0-R14 
	asm volatile(
		"MOV R0, %0;"
		"LDMFD R0, {R0-R14}" // CHECK IF R0 IS RIGHT
		:
		:"r"(cpu->next_context.r)
		:
	);

	// jump to saved PC and change MODE
	asm volatile(
		"MOVS PC, R14":::
	);
}

void start_scheduler() {
	context_t *sched = (get_cpu())->scheduler;

	proc_t *procs = get_procs();
	for(int i = 0; ; i = (i + 1) & (MAX_PROC_NUM - 1)) {
		// TODO: lock
		if (procs[i].state == RUNNABLE) {
			// TODO: set timer
			procs[i].state = RUNNING;
			// TODO: unlock

			asm volatile(
				"MOV R0, %0;"
				"STMFD R0, {R0-R13};"
				"MRS R0, CPSR;"
				"STR R0, [%0];"
				"STR PC, [%0, #56]"
				:
				:"r"(sched)
				:"r0"
			);

			switch_to(procs[i].context);
		}
		// TODO: unlock
	}
}