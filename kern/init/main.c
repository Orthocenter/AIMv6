/*
 * Copyright (C) 2015 Yuxin Chen <chenyuxin.mail@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drivers/serial/uart.h>
#include "kern/mm/page.h"
#include "kern/mm/page_test.h"
#include "kern/mm/slab.h"
#include "kern/mm/slab_test.h"
#include "kern/except/handler.h"
#include "kern/except/handler_test.h"

int start_kernel(void)
{
	uart_spin_puts("Hello from kernel!\r\n");
	
	init_pages();
	page_test();

	init_slabs();
	slab_test();

    handler_test();

	while(1);
}

