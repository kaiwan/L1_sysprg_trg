/*
 * sigq_snd.c
 * 
 * Author(s) : 
 * Kaiwan N Billimoria
 *  <kaiwan -at- kaiwantech -dot- com>
 * License(s): MIT
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>

#define SIGTOUSE SIGRTMIN+3

int main(int argc, char **argv)
{
	union sigval sv;
	char *endptr;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s pid-to-send-to value-to-send[int]\n",
			argv[0]);
		exit(1);
	}
	// sv.sival_int=atoi(argv[2]);  // simplistic; see below...

	/* Get an integer 'properly' checking for integer overflow etc;
	 * see the man page on strtol
	 */
	errno = 0;		/* To distinguish success/failure after call */
	sv.sival_int = strtol(argv[2], &endptr, 0);	// auto-detect the base
	if ((errno == ERANGE
	     && (sv.sival_int == LONG_MAX || sv.sival_int == LONG_MIN))
	    || (errno != 0 && sv.sival_int == 0)) {
		perror("strtol");
		exit(EXIT_FAILURE);
	}
	if (endptr == argv[2]) {
		fprintf(stderr, "No digits were found\n");
		exit(EXIT_FAILURE);
	}
	sv.sival_int &= 0xffffffff;	// int only
	if ((errno == ERANGE
	     && (sv.sival_int == INT_MAX || sv.sival_int == INT_MIN))
	    || (errno != 0 && sv.sival_int == 0)) {
		perror("strtol");
		exit(EXIT_FAILURE);
	}
	printf("%s: integer being transmitted: %d%s\n",
	       argv[0], sv.sival_int,
	       sv.sival_int < 0 ? " !WARNING! it's -ve (possible IoF)" : "");

	/* Use the sigqueue(3) to send a signal to a process, 'piggy-backing'
	 * some data along with it... 
	 * int sigqueue(pid_t pid, int sig, const union sigval value);
	 */
	if (sigqueue(atol(argv[1]), SIGTOUSE, sv) == -1) {
		//if (sigqueue(atol(argv[1]), SIGINT, sv) == -1) {
		perror("sigqueue failed");
		exit(1);
	}
	exit(0);
}
