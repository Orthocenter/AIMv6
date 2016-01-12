#include "handler.h"

#include <drivers/serial/uart.h>
#include <asm/irq.h>
#include <drivers/clock/gtc-a9mpcore.h>
#include "kern/sched/sched_cpu.h"
#include "kern/syscall/syscall.h"
#include "kern/sched/scheduler.h"

// this handler is executed after _SWI_handler
// SEE _SWI_handler for more details
//
// this handler runs in SYS mode on kern stack of proc
// R0 is set to be the bottom of the interrupt stack
// R1 is previous SP
// R2 is previous LR
// R3 is previous PC

void SWI_handler() {
    context_t ctx; // saved in the kern stack of proc
    context_t *ctx_ptr; // saved in the interrupt stack
    asm volatile(
        "STR R1, [R0, #56];" // save user SP to ctx_ptr->r[13]
        "STR R2, [R0, #60];" // save user LR to ctx_ptr->r[14]
        "STR R3, [R0, #64];" // save user PC to ctx_ptr->r[15]
        "MOV %0, R0;"
        :"=r"(ctx_ptr)
        :
        :"r0"
    );

    copy_context(&ctx, ctx_ptr);
    
    cpu_t *cpu = get_cpu();
    proc_t *proc = cpu->proc;
    proc->context = &ctx;
    proc->context->ttb = proc->ttb;

    u32 ret;

    switch(proc->context->r[0]) {
        case SBRK:
            uart_spin_puts("syscall: sbrk\r\n");
            ret = syscall_sbrk(proc);
            break;
        case FORK:
            uart_spin_puts("syscall: fork\r\n");
            break;
        case EXIT:
            uart_spin_puts("syscall: exit\r\n");
            break;
        case WAIT:
            uart_spin_puts("syscall: wait\r\n");
            break;
        case SLEEP:
            uart_spin_puts("syscall: sleep\r\n");
            break;
        case PUTS:
            uart_spin_puts("syscall: puts\r\n");
            ret = syscall_puts(proc);
            break;
        case PUTHEX:
            uart_spin_puts("syscall: puthex\r\n");
            ret = syscall_puthex(proc);
            break;
        default:
            uart_spin_puts("NO SUCH SYSTEM CALL ");
            uart_spin_puthex(proc->context->r[0]);
            // TODO: PANIC
    }

    proc->context->r[0] = ret; // FIXME: so ugly
    switch_to(proc->context);
}

void IRQ_handler() {
    context_t ctx; // saved in the kern stack
    context_t *ctx_ptr; // saved in the interrupt stack
    asm volatile(
        "STR R13, [R0, #-52];"
        "MOV %0, R0;"
        :"=r"(ctx_ptr)
        :
        :"r0"
    );
    copy_context(&ctx, ctx_ptr);

    cpu_t *cpu = get_cpu();
    proc_t *proc = cpu->proc;
    proc->context = &ctx;
    proc->context->ttb = proc->ttb;

    u32 id = get_irq_id();
    if (id == 27) {
        uart_spin_puts("received an IRQ from GTC\r\n");
        u64 gtc = gtc_get_time();
        uart_spin_puts("current gtc (H): ");
        uart_spin_puthex(gtc >> 32);
        uart_spin_puts("current gtc (L): ");
        uart_spin_puthex(gtc & 0xFFFFFFFF);

        proc->state = RUNNABLE;
    
        async_sleep_msec(10); // next round

        end_of_irq(id); // FIXME
        switch_to(cpu->scheduler);
    } else {
        uart_spin_puts("received an IRQ with id: ");
        uart_spin_puthex(id);
    }
}
