char str1[] = "Hello from user space\r\n";

int main() {
	char str2[] = "Hello again from user space\r\n";

	asm volatile(
		"LDR R0, =#6;"
		"MOV R1, %0;"
		"SWI #0"
		:
		:"r"(str1)
	);
	
	asm volatile(
		"LDR R0, =#6;"
		"MOV R1, %0;"
		"SWI #0"
		:
		:"r"(str2)
	);
	return 0;
}