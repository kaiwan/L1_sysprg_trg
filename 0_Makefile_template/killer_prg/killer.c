/*
 * killer.c
 *
 * Parent process traps a signal, then forks;
 * it then sends that same signal to it's child process;
 * child exits when signal arrives.
 * Conclusion: the child process *does* inherit the signal dispositions
 * of it's parent.
 *
 * Author :  Kaiwan N Billimoria, kaiwanTECH
 * License: MIT
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define SIG2SEND   SIGUSR1

/* Strictly speaking we should not use [f|s]printf() in a signal handler;
 * we do use it here in this demo for convenience...
 */
static void catchit(int signo)
{
	if (signo == SIG2SEND) {
		printf
		    ("catcher: process %d received signal %d; will now die.\n",
		     getpid(), SIG2SEND);
		exit(0);
	} else
		printf("%s: error, handler for %d only\n", __func__, SIG2SEND);
}

int main(int argc, char **argv)
{
	pid_t pid, n;
	struct sigaction act;
#define MAXTRIES  3
	int try = 0;

	memset(&act, 0, sizeof(act));
	act.sa_handler = catchit;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;

	if (sigaction(SIG2SEND, &act, 0) == -1) {
		perror("killer: sigaction failed.");
		exit(1);
	}

	switch ((pid = fork())) {
	case -1:
		perror("fork failed");
		exit(1);
	case 0:		// Child
		(void)pause();	/* wait for signal */
		exit(0);	/* never get here */
	default:		// Parent
		printf("parent is %d, child is %d\n", getpid(), pid);
		// poll for the child being alive
		while (kill(pid, 0) < 0) {
			printf(".");
			fflush(stdout);
			if (try >= MAXTRIES) {
				fprintf(stderr,
					"\n%s: parent: child still not alive, aborting..\n",
					argv[0]);
				exit(1);
			}
			(void)sleep(1);
			try++;
		}
		printf("\nSending signal #%d to child process (%d) now...\n",
		       SIG2SEND, pid);
		if (kill(pid, SIG2SEND) < 0) {
			perror("kill(2) failed");
			exit(1);
		}

		n = wait(0);
		if (n == -1) {
			perror("wait failed");
			exit(1);
		}
		printf("parent %d: child process fetched, wait "
		       "returned %d\n", getpid(), n);
	}
	return 0;
}
