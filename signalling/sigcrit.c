/*
 * sigcrit.c
 *
 * Here, we do some critical processing (just executing a "delay  loop" in 
 * this demo), while ensuring that certain signals are blocked out during 
 * that window.
 * POSIX-compliant functions and a delay loop used.
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * MIT License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include "../convenient.h"

static volatile sig_atomic_t c = 0, d = 0;

static void sighdlr(int signum)
{
	switch (signum) {
	case SIGINT:
		printf("SIGINT: count=%d\n", ++c);
		break;
	case SIGQUIT:
		printf("SIGQUIT: count=%d\n", ++d);
		break;
	default:
		printf("Signal %d: discarded\n", signum);
	}
	return;
}

int main(int argc, char **argv)
{
	struct sigaction act;
	sigset_t sigset, prevset;

	/* set up signals: trap SIGINT and SIGQUIT */
	memset(&act, 0, sizeof(act));
	act.sa_handler = sighdlr;
	sigfillset(&act.sa_mask);	/* block all signals while handling */
	act.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &act, 0) == -1) {
		perror(argv[0]);
		exit(1);
	}
	if (sigaction(SIGQUIT, &act, 0) == -1) {
		perror(argv[0]);
		exit(1);
	}
	fprintf(stderr, "Signals SIGINT and SIGQUIT trapped..\n");

	/* Block SIGINT & SIGQUIT from occuring from now on..
	 * sigblock  -- old interface
	 * POSIX - compilant interface is:
	 * int sigprocmask(int how, const sigset_t *set, 
	 *          sigset_t *oldset);
	 */
#if 1
	sigemptyset(&sigset); // clear all bits
	if (sigaddset(&sigset, SIGINT) == -1)      // sets the bit for SIGINT
		perror("sigaddset failure"), exit(1);
	if (sigaddset(&sigset, SIGQUIT) == -1)      // sets the bit for SIGQUIT
		perror("sigaddset failure"), exit(1);
#else
	sigfillset(&sigset);
#endif
	if (sigprocmask(SIG_BLOCK, &sigset, &prevset) == -1)
		perror("sigprocmask 1 failure"), exit(1);

	//------------- Critical code goes here..
	printf("Entering critical section now...\n");
	DELAY_LOOP(65, 500);	// emulate processing: prints ASCII 65 ("A") 500 times
	printf("Exiting critical section now...\n");
	//------------- Critical code done; reset signal settings..

	/* sigsetmask( oldmask ); --old interface 
	 * Restore signal mask to previously saved state..
	 */
	if (sigprocmask(SIG_SETMASK, &prevset, 0) == -1)
		perror("sigprocmask 2 failure"), exit(1);

	while (1)
		(void)pause();
}
