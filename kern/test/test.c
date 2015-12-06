#include "test.h"

void panic() {
	while(1) ;
}

void expect(int val, int exp) {
	expect_no_newline(val, exp);
	uart_spin_puts("\r\n");
}

void expect_no_newline(int val, int exp) {
	if (val == exp) {
		uart_spin_puts("OK");
	} else {
		uart_spin_puts("FAILED\r\n");
		uart_spin_puts("got: ");
		uart_spin_puthex(val);
		uart_spin_puts("expected: ");
		uart_spin_puthex(exp);

		panic();
	}
}
