/*
 * zombie.c
 * Small fork() demo program to generate a zombie;
 * Run this in the background and look up the status field with "ps -l";
 *  should be "Z".
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * License: MIT
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	pid_t p;

	p = fork();
	switch (p) {		/* often coded as switch( (p=fork()) ) */
	case -1:
		perror("fork failed"), exit(1);
	case 0:		// Child
		printf
		    ("Child pid %d : p=%d; exiting now (generating a zombie!)..\n",
		     getpid(), p);
		exit(0);
	default:		// Parent
#if 0
		wait(0); // no zombie as am waiting...
#endif
		printf("Parent pid %d : p=%d; sleeping now for 300s without wait()-ing \
(resulting in an orphaned child)..\n", getpid(),
		       p);
		sleep(300); // lets take a nap
		// Unless you kill the parent, the child remains a zombie!
		printf("Parent: sleep done, exiting. Notice how (on Linux) the \
zombie is now cleared!\n");
		exit(0);
	}
}
