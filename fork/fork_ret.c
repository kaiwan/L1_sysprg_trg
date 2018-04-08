
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	pid_t ret = 0;

	switch (ret = fork()) {
	case -1:		// failure case
		//  <clean-up code>
		perror("fork failed");
		exit(1);
	case 0:		// the Child process
		printf("Child:  PID = %d, return value = %d\n", getpid(), ret);
		//child_work(); // everything the child has to do
		exit(0);
	default:		// the Parent process
		printf("Parent: PID = %d, return value = %d\n", getpid(), ret);
		wait(0);
		//parent_work(); // everything the parent has to do
		/* -OR- we could just drop through the case... */
	}
	exit(0);
}
