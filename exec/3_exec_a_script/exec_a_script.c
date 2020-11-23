/*
 * exec_poc.c
 *
 * Trivial demo of using execl() to execute a process.
 * Shows that the PID of the predecessor and successor are the same.
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * License: MIT
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char *script = "./tiny_script.sh";
	
	printf("Predecessor \"%s\": PID is %d\n", argv[0], getpid());
	printf("%s: will exec the bash script \"%s\" now..\n", argv[0], script);

	if (execl("/bin/bash", "bash", "-c", script, (char *)0) == -1) {
		perror("execl failed");
		exit(1);
	}
	// this line will never be reached on execl() success
	exit(0);		// just to get rid of compiler warning
}
