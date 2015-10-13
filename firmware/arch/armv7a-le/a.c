int _start() {
	void (*puts)(const char *) = (void *) 0x1ff0000c;
	
	puts("hello world\n");

	return 0;
}
