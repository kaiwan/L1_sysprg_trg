
#include <stdio.h>

int main() {
    unsigned long val;

	printf("hello, inline assembly world\n");
    __asm__ volatile (
        "mov %%rax, %0"
        : "=r"(val)        // output operand
        :                  // no input
        :                  // no clobbered registers
    );

    printf("Value in RAX: %lu\n", val);

	printf("now attempting HLT instruction!\n");
    __asm__ volatile ("hlt");

    return 0;
}
