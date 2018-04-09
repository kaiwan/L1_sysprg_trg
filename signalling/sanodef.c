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

int indx = 0;
unsigned char buf[] = { '0', '1', '2', '3', '4', '5', '6', '7' };

#define		MAX		8

void *stack(void)
{
	if (__WORDSIZE == 32) {
		__asm__("movl %esp, %eax");
	} else if (__WORDSIZE == 64) {
		__asm__("movq %rsp, %rax");
	}
/* Accumulator holds the return value */
}

static void sighdlr(int signum)
{
	static int s = 0;
	int saved;

	MSG_SHORT("\nsighdlr: caught signal %d,", signum);

	switch (signum) {
	case SIGINT:
		s++;
		if (s >= MAX)
			s = 1;
		saved = s;

		MSG_SHORT(" s=%d ; stack %p :", s, stack());
		DELAY_LOOP(saved + 48, 30);	/* +48 to get the equivalent ASCII value */
		MSG_SHORT("*");
		(void)fflush(stdout);
		break;

	default:;
	}
	return;
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

	if (atoi(argv[1]) == 1)
		flags = SA_NODEFER | SA_RESTART;
	else if (atoi(argv[1]) == 0)
		flags = SA_RESTART;

	// set up signal handler
	memset(&act, 0, sizeof(act));
	act.sa_handler = sighdlr;
	sigemptyset(&act.sa_mask);
	act.sa_flags = flags;

	if (sigaction(SIGINT, &act, 0) == -1) {
		perror(argv[0]);
		exit(1);
	}

	fprintf(stderr, "\nProcess awaiting signals\n");
	while (1)
		(void)pause();
}
