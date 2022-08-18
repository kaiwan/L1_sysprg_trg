/*
 * nano.c
 * 
 * 
 * Compile with:
 *  gcc nano.c -o nano -Wall [-Os]
 *
 * Author :  Kaiwan N Billimoria, kaiwanTECH
 * License(s): MIT
 */
extern int errno;

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include "../convenient.h"

void sig(int signum)
{
	printf("**Signal %d interruption!**\n", signum);
}

int main(int argc, char **argv)
{
	struct sigaction act;
	int nsec = 10, ret;
	struct timespec req, rem;

	if (argc == 1) {
		fprintf(stderr, "Usage: %s option=[0|1]\n\
0 : uses the sleep(3) function\n\
1 : uses the nanosleep(2) syscall\n", argv[0]);
		exit(1);
	}

	/* set up signals: trap SIGINT and SIGQUIT */
	memset(&act, 0, sizeof(act));
	act.sa_handler = sig;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &act, 0) || sigaction(SIGQUIT, &act, 0)
	    == -1) {
		perror("sigaction failure");
		exit(1);
	}

	/* TODO: use strto[u]l() to do proper IoF checks ! */
	if (atoi(argv[1]) == 0) {	/* sleep */
		printf("sleep for %d s now...\n", nsec);
		ret = sleep(nsec);
		 /* Actually BUGGY !!
		 Why? As the sleep can be aborted by a signal and we don't check
		 the return value from sleep() - the # of seconds remaining to sleep!
		 */
		printf("sleep returned %u\n", ret);
	} else if (atoi(argv[1]) == 1) {	/* nanosleep - the RIGHT way */
		req.tv_sec = nsec;
		req.tv_nsec = 0;
		while ((nanosleep(&req, &rem) == -1) && (errno == EINTR)) {
			printf("nanosleep interrupted: rem time: %07lu.%07lu\n",
			       rem.tv_sec, rem.tv_nsec);
			req = rem;
		}
	}
	exit(0);
}
