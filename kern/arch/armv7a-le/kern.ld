ENTRY(init_cpu)

SECTIONS
{
	kern 2M : {
		kern/except/vector.o
		*/init/cpu.o
		*/init/l1cache.o
		*/init/mmu.o
		*/init/l2cache.o
		drivers/serial/uart.o
		drivers/serial/uart-zynq7000.o
		*.o
	}

	test 10M : {
		kern/test/test_user_helloworld.o
	}
}
