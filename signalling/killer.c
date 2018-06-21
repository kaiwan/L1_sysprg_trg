/* 
 * killer.c
 * Parent process forks and sends signal to child process;
 * child exits when signal arrives
 *
 * Author :  Kaiwan N Billimoria, kaiwanTECH
 * License(s): MIT
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

/* Strictly speaking we should not use [f|s]printf() in a signal handler;
 * we do use it here in this demo for convenience...
 */
static void catchit(int signo)
{
	if (signo == SIGINT) {
		printf
		    ("catcher: process %d received signal SIGINT ; will now die.\n",
		     getpid());
		exit(0);
	} else
		printf("catchit: error, handler for SIGINT only\n");
}

int main(int argc, char **argv)
{
	pid_t pid, n;
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = catchit;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &act, 0) == -1) {
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
		(void)sleep(1);

		if (kill(pid, 0) == 0) {	// iff child is alive ...
			/* send signal to child */
			if (kill(pid, SIGINT) == -1) {
				perror("kill failed");
				exit(1);
			}
		}

		if ((n = wait(0)) == -1) {
			perror("wait failed");
			exit(1);
		}
		printf("parent %d: child process fetched, wait "
		       "returned %d\n", getpid(), n);
	}
	return 0;
}
