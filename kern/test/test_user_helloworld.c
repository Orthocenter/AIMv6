static char str1[] = "Hello from the other side\r\n";

int main() {
	static char str2[] = "Hello again from user space\r\n";
	asm volatile(
		"LDR R0, =#6;"
		"MOV R1, %0;"
		"SWI #0"
		:
		:"r"(str1)
	);
	
	while(1) {
		asm volatile(
			"LDR R0, =#6;"
			"MOV R1, %0;"
			"SWI #0"
			:
			:"r"(str2)
			);
	}
	return 0;
}