/*
 * sigq_recv.c
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

/*---------------- Macros -------------------------------------------*/
#define SIGTOUSE SIGRTMIN+3

/*---------------- Typedef's, constants, etc ------------------------*/

/*---------------- Functions ----------------------------------------*/

static void catcher(int signum, siginfo_t *si, void *ctx)
{
	printf("%d: received signal # %d from %d:RUID %d\n"
	       "Value recieved: %d\n",
	       getpid(), signum, si->si_pid, si->si_uid,
	       si->si_value.sival_int);
}

int main(int argc, char **argv)
{
	struct sigaction act;

	act.sa_sigaction = catcher;
	sigemptyset(&act.sa_mask);	// while handling the signal, allow all
	//sigfillset(&act.sa_mask);     // while handling the signal, disallow all
	act.sa_flags = SA_RESTART | SA_SIGINFO;
	if (sigaction(SIGTOUSE, &act, 0) == -1) {
		//if (sigaction(SIGINT, &act, 0) == -1) {
		perror("sigaction failure");
		exit(1);
	}

	printf("Hey, in %s[%d]! Pausing now ... [^C to exit..]\n", argv[0],
	       getpid());
	while (1)
		pause();
	exit(0);
}

/* vi: ts=4 */
