ENTRY(init_cpu)

SECTIONS
{
	low 2M : {
		*/init/cpu.o
		*/init/l1cache.o
		*/init/mmu.o
		*/init/l2cache.o
		drivers/serial/uart.o
		drivers/serial/uart-zynq7000.o
	}
	
	high 2048M : AT( ADDR(low) + SIZEOF(low) ) {
		*.o
	}
}
