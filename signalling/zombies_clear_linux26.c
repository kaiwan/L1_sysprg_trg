/*
 * clear_zombie_linux26.c
 *
 * A process forks; the parent does not wait for the child,
 * it continues to do some work...
 * When the child does die, the parent should nonetheless clear
 * the resulting zombie.
 *
 * We do this here using one of two really easy! approaches:
 * (a) Linux 2.6: set the SA_NOCLDWAIT flag (within the sigaction()
 * trapping SIGCHLD); the kernel then ensures that terminated
 * child(ren) do not become zombies.
 * (b) _even simpler_: just ignore the SIGCHLD signal.
 *
 * Author :  Kaiwan N Billimoria, kaiwanTECH
 * License(s): MIT
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../convenient.h"

#if 0
/* SIGCHLD handler */
static void clearzomb(int signum)
{
	pid_t pid;

	MSG("\nPID: %d\n", getpid());
	// wait(0);
	/*
	   pid_t wait3(int *status, int options,
	   struct rusage *rusage);

	   Why use wait3 in a loop?
	   1. We should not actually wait (block) for any other child(ren) that are 
	   still alive; therefore use the WNOHANG option.
	   2. More importantly, if this "server" were busy, we can have a scenario where
	   many children die (almost) simultaneously; the SIGCHLD of the first dead 
	   child will be processed but the other signals will be lost (UNIX behaviour:
	   while handling a signal, the kernel will have masked that signal).
	   To guarantee that we actually process all dead children - and clear all 
	   zombies - we must, therefore, call the wait3 in a loop, until it fails.
	 */
	while ((pid = wait3(0, WNOHANG, 0)) != -1)
		MSG("\n** Zombie %d cleared **\n", pid);
}
#endif

int main(int argc, char **argv)
{
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART | SA_NOCLDSTOP;  /* SA_NOCLDSTOP: no SIGCHLD on stop of child(ren) */
	act.sa_flags |= SA_NOCLDWAIT;	/* 2.6 Linux: prevent zombie on termination of child(ren)! */
	if (sigaction(SIGCHLD, &act, 0) == -1) {
		perror("sigaction failed");
		exit(1);
	}

	printf("parent: %d\n", getpid());
	switch (fork()) {
	case -1:
		perror("fork failed");
		exit(1);
	case 0:		// Child
		printf("child: %d\n", getpid());
		DELAY_LOOP('c', 250);
		exit(0);	// child dies before parent...
	default:	// Parent
		/* don't call wait*(); implies the child will become a zombie on exit */
		while (1)
			pause();
	}
	exit(0);
}
