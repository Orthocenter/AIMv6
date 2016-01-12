ENTRY(init_cpu)

SECTIONS
{

	test 0x80a00000 : AT(0xa00000){
		kern/test/test_user_helloworld.o
	}
	
	kern 0x80200000 : AT(0x200000) {
		kern/except/vector.o
		*/init/cpu.o
		*/init/l1cache.o
		*/init/mmu.o
		*/init/l2cache.o
		drivers/serial/uart.o
		drivers/serial/uart-zynq7000.o
		*.o
	}

}
