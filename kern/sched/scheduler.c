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

	// load banked R13 R14
	asm volatile(		
		"MOV R0, %0;"
		"LDMFD R0, {R13-R14}" 
		:
		:"r"(cpu->next_context.r+13)
		:
	);

	// switch to SVC mode
 	asm volatile(
        "MRS R12, CPSR;"
        "BIC R12, R12, #0x1F;"
        "ORR R12, R12, %0;"
        "MSR CPSR_c, R12"
        :
        :"r"(0b10011)
    );

 	// save next_context.cpsr to spsr for further restoring
    asm volatile (
    	"MSR SPSR, %0;"
    	:
    	:"r"(cpu->next_context.cpsr)
    	:
    );

    // load next_context.r[PC] to LR
    asm volatile(
    	"MOV r0, %0;"
    	"LDMFD R0, {R14}"
    	:
    	:"r"(cpu->next_context.r+15)
    	:
    );

    // load general registers
	asm volatile(		
		"MOV R0, %0;"
		"LDMFD R0, {R0-R12}"
		:
		:"r"(cpu->next_context.r)
		:
	);

	// jump to next_context.PC and restore CPSR
	asm volatile(
		"MOVS PC, LR"
	);
}

void* get_start_scheduler() {
	return start_scheduler;
}

static void start_scheduler() {
	context_t *sched = (get_cpu())->scheduler;

	proc_t *procs = get_procs();
	for(int i = 0; ; i = (i + 1) & (MAX_PROC_NUM - 1)) {
		// TODO: lock
		if (procs[i].state == RUNNABLE) {
			// TODO: set timer
			procs[i].state = RUNNING;
			// TODO: unlock

			// save CPSR
			asm volatile(
				"MOV R0, %0;"
				"MRS R0, CPSR;"
				"STR R0, [%0];"
				:
				:"r"(sched)
				:"r0"
			);
			//save R0 - R0-R15
			asm volatile(
				"MOV R0, %0;"
				"STMFD R0, {R0-R14};"
				"STR PC, [%0, #60]" // point to the instruction after switch_to
				:
				:"r"(sched->r)
				:"r0"
			);

			switch_to(procs[i].context);
		}
		// TODO: unlock
	}
}