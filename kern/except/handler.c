#include "handler.h"

#include <drivers/serial/uart.h>
#include <asm/irq.h>
#include <drivers/clock/gtc-a9mpcore.h>

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

void SWI_handler() {
    uart_spin_puts("print from SWI handler before changing mode\r\n");
    change_mode(SYS_MODE);
    uart_spin_puts("print from SWI handler\r\n");
    change_mode(SVC_MODE);
}

void IRQ_handler() {
    uart_spin_puts("print from IRQ handler before changing mode\r\n");

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

    change_mode(SYS_MODE);
    uart_spin_puts("print from IRQ handler\r\n");
    change_mode(IRQ_MODE);

    end_of_irq(id);
}
