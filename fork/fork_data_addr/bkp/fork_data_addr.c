/*
 * fork_data_addr.c
 * Demo to show that data is copied across the fork (and not shared.
 * The COW optimization of course is till present under the hood).
 *
 * Also, the _virtual_ addresses might be the same in parent & child,
 * but the underlying physical addresses will differ once the memory
 * is written to.
 *
 * Created on: Mar 5, 2011
 * Author: Kaiwan NB
 * kaiwan at kaiwantech dot com
 * license: MIT
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define FEW   16
static char *p;

#define PR(pi, pj)   do { \
printf ("   i=%d j=%d\n" \
		"   &i=%p &j=%p\n", pi, pj, (void *)&pi, (void *)&pj); \
} while (0)

int main(int argc, char **argv)
{
	pid_t ret = 0;
	int i = 5, j = 6, pgsz = getpagesize(); // typically returns 4096

	p = malloc(pgsz);
	if (!p) {
printf("malloc failed, aborting!\n");
exit(1);
	}

	switch (ret = fork()) {
	case -1:		// failure case
		perror("fork failed");
		exit(1);
	case 0:		// the Child process
		printf("\nChild: before var update: ");
		PR(i, j);
		i--;
		j++;
		printf("Child: after var update: ");
		PR(i, j);

		memset(p, 'c', pgsz);
		printf("\nChild: malloc ret: %p\n", p);
		{
			int k;

			for (k = 0; k < FEW; k++)
				printf("%c ", p[k]);
			printf("\n");
		}
		//pause();
#if 0
/*
 * https://stackoverflow.com/questions/23440132/fork-after-malloc-in-parent-does-the-child-process-need-to-free-it
 * "You do not need to explicitly free anything in the child process; in fact (because of the COW thing) it's not a bright idea to do so."
 * [ ... ]
 * "Calling free before execvp is pointless, and it actually makes your code less
 * reusable/portable, since it's not valid to call free in the child after fork
 * if the calling process is multithreaded."
 */
		free(p);
#endif
		exit(0);
	default:		// the Parent process
//		wait(0);	// synchronize: let the child run first

		printf("\nParent: before var update: ");
		PR(i, j);
		printf("\nParent: after var update: ");
		i++;
		j--;
		PR(i, j);

		memset(p, 'p', pgsz);
		printf("\nParent: malloc ret: %p\n", p);
		{
			int k;

			for (k = 0; k < FEW; k++)
				printf("%c ", p[k]);
			printf("\n");
		}
		//pause();
		free(p);

		break;
	}
	/* The pause() allows you to see the detailed process VAS via /proc/PID/maps or via the
	 * procmap util!
	 * https://github.com/kaiwan/procmap
	 */
	//pause(); 
	exit(0);
}
