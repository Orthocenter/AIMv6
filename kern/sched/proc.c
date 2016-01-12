#include "proc.h"

#include "kern/mm/slab.h"
#include "kern/mm/upgtbl.h"

void init_procs() {
	for(int i = 0; i < MAX_PROC_NUM; i++) {
		procs[i].state = UNUSED;	
	}
}

proc_t *get_procs() {
	return procs;	
}

proc_t *create_proc() {
	proc_t *proc;
	// lock
	for(int i = 0; i < MAX_PROC_NUM; i++) {
		if(procs[i].state == UNUSED) {
			proc = procs + i;
			break;
		}
	}	

	proc->state = EMBRYO;
	//unlock

	proc->pid = cpid++;

	proc->parent = NULL;

	proc->ttb = create_page_table();

	proc->context = alloc_obj(sizeof(context_t));
	proc->context->cpsr = DEFAULT_PROC_CPSR;
	proc->context->r[13] = proc->context->r[12] = DEFAULT_PROC_USER_STACK;
	proc->context->r[14] = 0x00000000;
	proc->context->ttb = proc->ttb;

	proc->killed = 0;

	return proc;
}