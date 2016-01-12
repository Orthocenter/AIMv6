#include "except.h"

#include <asm/irq.h>
#include <asm/except.h>
#include <asm/mem.h>
#include "kern/except/handler.h"
#include <drivers/serial/uart.h>

void set_sp(u32 mode, u32 ret_mode, u32 sp) {
    asm volatile(
        "MOV R8, FP"
    );

    u32 r12;
    asm volatile(
        "MRS R12, CPSR;"
        "BIC R12, R12, #0x1F;"
        "ORR R12, R12, %0;"
        "MSR CPSR_c, R12"
        :
        :"r"(mode)
    );

    asm volatile(
        "MOV SP, %0;"
        :
        :"r"(sp)
        :
    );

    asm volatile(
        "MRS R12, CPSR;"
        "BIC R12, R12, #0x1F;"
        "ORR R12, R12, %0;"
        "MSR CPSR_c, R12"
        :
        : "r"(ret_mode)
        : "r12"
    );

    asm volatile(
        "MOV FP, R8"
    );
}

void init_handler() {
    // set vector table base
    asm volatile(
        "mov r0, %0;"
        "mcr p15, 0, r0, c12, c0, 0"
        :
        :"r"(KERN_BASE + KERN_BASE_PHY)
    );

    // set IRQ SP
    uart_spin_puts("EXCEPT: setting IRQ SP\r\n");
    set_sp(IRQ_MODE, SYS_MODE, IRQ_STACK);
    uart_spin_puts("EXCEPT: IRQ SP set\r\n");

    // set SVC SP
    uart_spin_puts("EXCEPT: setting SVC SP\r\n");
    set_sp(SVC_MODE, SYS_MODE, SVC_STACK);
    uart_spin_puts("SVC SP set\r\n");
}
