#include "handler_test.h"
#include <drivers/serial/uart.h>
#include "asm/except.h"

void handler_test() {
    uart_spin_puts("EXCEPT/handler: start interrupt handlers test\r\n");
    uart_spin_puts("EXCEPT/handler: test SWI: ");
    asm volatile (
        "SWI #0"
    );
    uart_spin_puts("OK");
}
