/* vaddr_process.c
 *
 * Display the virtual addresses of different regions of the process VAS.
 */
//extern char *environ;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#undef BITS_32

#ifdef BITS_32
#define FMTSPC "%08x"
#define TYPECST unsigned int
#else				/* 64-bit */
#define FMTSPC "%016lx"
#define TYPECST unsigned long
#endif
typedef unsigned int u32;

int gi = 5, gu;

int main(int argc, char **argv)
{
	char local = 0xff, *heapmem;
	int i = 1, j = 2, k = 3;

	heapmem = malloc(1024);
	printf("PID %d: virtual addresses:"
	       "\n&main    = 0x" FMTSPC
	       "\n&gi      = 0x" FMTSPC
	       "\n&gu      = 0x" FMTSPC
	       "\n&heapmem = 0x" FMTSPC
	       "\n&local   = 0x" FMTSPC
	       "\n&environ = 0x" FMTSPC "\n",
	       getpid(), (TYPECST) & main, (TYPECST) & gi, (TYPECST) & gu,
	       (TYPECST) heapmem, (TYPECST) & local, (TYPECST) environ);
	printf("\n... In pause now... ^C to end (can run procmap !)\n");
	pause();
	free(heapmem);
	exit(0);
}
