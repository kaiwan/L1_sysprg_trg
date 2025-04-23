/*
 * fork2c.c
 * (c) kaiwan nb, kaiwanTECH
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Here, we're simply displaying all the macro ret values. The right way though,
 * is to only display some of them when a certain condition's met. 
 * F.e. reg the WTERMSIG(): only call it if WIFSIGNALED() returns True.
 * See the man page on wait(2) for the details.
 */
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
		sleep(sleep_time);
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

	switch (n = fork()) {
	case -1:
		perror("fork");
		exit(1);
	case 0:		// Child
		printf("Child 1 PID %d sleeping for %ds...\n", getpid(),
		       c1_slptm);
		sleep(c1_slptm);
		exit(0);
	default:		// Parent
		createChild(c2_slptm);
		while ((cpid = wait(&stat)) != -1)
			displayChildStatus(stat, cpid);
	}
	printf("\nParent: all children dead, exiting...\n");
	exit(0);
}
