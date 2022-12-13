/*
 * chld_sig.c
 * Hypothesis: when a parent process receives a signal, it's also delivered
 * to all of it's children
 * Experiment: a parent traps the signal SIGUSR1 and creates two children; we
 * use the simple script 'sigit' to send SIGUSR1 to the parent process only.
 * The children do Not receive the signal
 * $ ./chld_sig 40 40 &
 * $ ./sigit
 * ...
 * Conclusion:
 * Only the process where the signal is delivered - the PID - receives it.
 * Don't confuse this with the fact that children inherit the signal dispositions
 * (handlers) of the parent!
 *
 * (c) kaiwan nb, kaiwanTECH
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

static void catcher(int sig)
{
	printf("**************** %s():sig %d ****************\n", __func__, sig);
}

static void sleep_r(int sec)
{
	unsigned int rem = sec;
	do {
		rem = sleep(rem);
		printf("%s():rem=%d\n", __func__, rem);
	} while (rem);
}

static void displayChildStatus(int stat, pid_t cpid)
{
	printf("\nChild %d just died!\n"
	       "WIFEXITED : %d\n"
	       "Exit status: %d\n"
	       "WIFSIGNALED : %d\n"
	       "Termination signal: %d\n"
	       "WIFSTOPPED : %d\n"
	       "Stop signal: %d\n"
	       "WCOREDUMP: %d\n"
	       "WIFCONTINUED: %d\n",
	       cpid,
	       WIFEXITED(stat),
	       WEXITSTATUS(stat),
	       WIFSIGNALED(stat),
	       WTERMSIG(stat),
	       WIFSTOPPED(stat),
	       WSTOPSIG(stat), WCOREDUMP(stat), WIFCONTINUED(stat)
	    );
}

static void createChild(int sleep_time)
{
	pid_t n;

	switch (n = fork()) {
	case -1:
		perror("fork");
		exit(1);
	case 0:		// Child
		printf("Child 2 PID %d sleeping for %ds...\n", getpid(),
		       sleep_time);
		sleep_r(sleep_time);
		exit(0);
	default:
		;		// Parent returns..
	}
}

static int chkrange(int num, int min, int max)
{
	if (num < min)
		return -2;
	else if (num > max)
		return -3;
	return 0;
}

int main(int argc, char **argv)
{
	int stat = 0;
	unsigned short int c1_slptm = 3, c2_slptm = 7;
	pid_t n, cpid;
	struct sigaction act;

	if (argc == 3) {
		c1_slptm = atoi(argv[1]);
		if (chkrange(c1_slptm, 1, 60) < 0) {
			fprintf(stderr,
				"%s: ensure first param [%u] is within acceptable"
				" range [1-60]\n", argv[0], c1_slptm);
			exit(1);
		}
		c2_slptm = atoi(argv[2]);
		if (chkrange(c2_slptm, 1, 60) < 0) {
			fprintf(stderr,
				"%s: ensure second param [%u] is within acceptable"
				" range [1-60]\n", argv[0], c2_slptm);
			exit(1);
		}
	}

	// Signals
	memset(&act, 0, sizeof(act));
	act.sa_handler = catcher;
	sigemptyset(&act.sa_mask);	/* allow all signals while in the handler 
					 * (with the execption of the signal being handled) */
	act.sa_flags = 0;	/* the sigemptyset() and flags init to 0 are redundant 
				 * due to the memset(), but we're just showing proper form */
	if (sigaction(SIGUSR1, &act, 0) == -1) {
		perror("sigaction failure");
		exit(1);
	}


	switch (n = fork()) {
	case -1:
		perror("fork");
		exit(1);
	case 0:		// Child
		printf("Child 1 PID %d sleeping for %ds...\n", getpid(),
		       c1_slptm);
		sleep_r(c1_slptm);
		exit(0);
	default:		// Parent
		createChild(c2_slptm);
		while ((cpid = wait(&stat)) != -1)
			displayChildStatus(stat, cpid);
	}
	printf("\nParent: all children dead, exiting...\n");
	exit(0);
}
