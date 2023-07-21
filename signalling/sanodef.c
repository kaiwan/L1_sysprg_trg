/*
 * sanodef.c
 *
 * We trap a signal and don't block out signal delivery of the same
 * (or other) signals during signal handling (with the SA_NODEFER flag).
 * Find that as signals hit the process, the handler is called repeatedly
 * (reentering the handler code) over & over again until all signals
 * are handled.
 *
 * Remember to compile with optimization Off (-O0) so as to have the
 * delay loop work correctly.
 *
 * Author :  Kaiwan N Billimoria, kaiwanTECH
 * License(s): MIT
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/file.h>
#include "../convenient.h"

#define	MAX	8

void *stack(void)
{
// arch name ref: https://sourceforge.net/p/predef/wiki/Architectures/
#ifdef __x86_64__
	printf("X86:\n");
	if (__WORDSIZE == 32) {
		__asm__("movl %esp, %eax");
	} else if (__WORDSIZE == 64) {
		__asm__("movq %rsp, %rax");
	}
	/* Accumulator holds the return value */
#endif
#ifdef __arm__
	asm volatile("mov r0, sp"); // r0 holds the ret val
#endif
#ifdef __aarch64__
	asm volatile("mov x0, sp"); // x0 holds the ret val
#endif
}

static void sighdlr(int signum)
{
	static volatile sig_atomic_t s;
	int saved;

	printf("\nsighdlr: caught signal %d,", signum);

	switch (signum) {
	case SIGINT:
		s++;
		if (s >= MAX)
			s = 1;
		saved = s;

		printf(" s=%d ; stack %p :", s, stack());
		DELAY_LOOP(saved + 48, 30);	/* +48 to get the equivalent ASCII value */
		printf("*");
		(void)fflush(stdout);
		break;

	default:
		;
	}
}

int main(int argc, char **argv)
{
	int flags = 0;
	struct sigaction act;

	if (argc == 1) {
		fprintf(stderr, "Usage: %s [option]\n\
option=0 : don't use SA_NODEFER flag (default sigaction style)\n\
option=1 : use SA_NODEFER flag (will process signal immd)\n", argv[0]);
		exit(1);
	}

	printf("SA_NODEFER used? %s\n", (atoi(argv[1])==0?"no":"yes"));
	if (atoi(argv[1]) == 1)
		flags = SA_NODEFER | SA_RESTART;
	else if (atoi(argv[1]) == 0)
		flags = SA_RESTART;

	// set up signal handler
	memset(&act, 0, sizeof(act));
	act.sa_handler = sighdlr;
#if 1 // keep this
	sigemptyset(&act.sa_mask); // allow all other signals...
#else
	sigfillset(&act.sa_mask); // don't allow all other signals...
#endif
	act.sa_flags = flags;

	if (sigaction(SIGINT, &act, 0) == -1) {
		perror(argv[0]);
		exit(1);
	}

	fprintf(stderr, "\nProcess awaiting signals\n");
	while (1)
		(void)pause();
}
