void (*puts)(const char *) = (void *)0x1ff0000c;

void sayhello() {
	puts("Hello world from partition2!\r\n");
}
