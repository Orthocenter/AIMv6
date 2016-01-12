#include "sched_cpu.h"

u32 get_cpu_id() {
	u32 cpu_id = 0xF;
 	asm volatile (
 		"MRC p15, 0, %0, c0, c0, 5"
 		: "=r" (cpu_id)
 		:
 		:
 	);

 	return cpu_id & 0x3;
}

cpu_t *get_cpu() {
	u32 cpu_id = get_cpu_id();

	return cpus + cpu_id;
}