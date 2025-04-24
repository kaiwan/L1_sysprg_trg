/*
 * ==============================================================================
 * fork_test.c
 * C Code for fork() creation test
 * Hey, the overhead's not as bad as it first appears, as the child process
 * isn't alive for any significant amount of time...
 * ==============================================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NFORKS 50000

void do_nothing()
{
	int i;
	i = 0;
}

int main()
{
	int pid, j, status;

	for (j = 0; j < NFORKS; j++) {

		if ((pid = fork()) < 0) {
			printf("fork failed with error code= %d\n", pid);
			exit(1);
		}

  /*** this is the child of the fork ***/
		else if (pid == 0) {
			do_nothing();
			exit(0);
		}

  /*** this is the parent of the fork ***/
		else {
			waitpid(pid, &status, 0);
		}
	}
	exit(0);
}
