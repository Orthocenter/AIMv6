#include "handler.h"

#include <drivers/serial/uart.h>
#include <asm/irq.h>
#include <drivers/clock/gtc-a9mpcore.h>
#include "kern/sched/sched_cpu.h"
#include "kern/syscall/syscall.h"
#include "kern/sched/scheduler.h"

void change_mode(u32 mode) {
    int cpsr;
    asm volatile(
        "mrs r0, cpsr\r\n"
        "mov %0, r0" : "=r"(cpsr));
    uart_spin_puts("CPSR=");
    uart_spin_puthex(cpsr);

    asm volatile(
        "MRS R0, CPSR;"
        "BIC R0, R0, #0x1F;"
        "ORR R0, R0, %0;"
        "MSR CPSR, R0"
        :
        : "r"(mode)
        : "r0"
    );
}

// SYS mode, running on kern stack
void SWI_handler() {
    context_t ctx; // saved in the kern stack
    context_t *ctx_ptr; // saved in the interrupt stack
    asm volatile(
        "STR R1, [R0, #56];" // save user SP to ctx_ptr->r[13]
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
            uart_spin_puts("sbrk\r\n");
            ret = syscall_sbrk(proc);
            break;
        case FORK:
            uart_spin_puts("fork\r\n");
            break;
        case EXIT:
            uart_spin_puts("exit\r\n");
            break;
        case WAIT:
            uart_spin_puts("wait\r\n");
            break;
        case SLEEP:
            uart_spin_puts("sleep\r\n");
            break;
        case PUTS:
            uart_spin_puts("puts\r\n");
            ret = syscall_puts(proc);
            break;
        case PUTHEX:
            uart_spin_puts("puthex\r\n");
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
    } else {
        uart_spin_puts("received an IRQ with id: ");
        uart_spin_puthex(id);
    }

    end_of_irq(id); // FIXME
}
