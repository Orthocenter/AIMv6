#include "handler_test.h"
#include <drivers/serial/uart.h>
#include <drivers/clock/gtc-a9mpcore.h>
#include "asm/except.h"

void handler_test() {
	u64 comp = 0x925BED6El;
	uart_spin_puts("EXCEPT/handler: setting comp at ");

	uart_spin_puthex(comp);
	set_gtc_comparator(comp);

    // uart_spin_puts("EXCEPT/handler: start interrupt handlers test\r\n");
    // uart_spin_puts("EXCEPT/handler: test SWI: ");
    // asm volatile (
        // "SWI #0"
    // );
    uart_spin_puts("OK");
}
