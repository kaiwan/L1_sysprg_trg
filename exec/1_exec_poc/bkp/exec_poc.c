/*
 * exec_poc.c
 *
 * Trivial demo of using execl() to execute a process.
 * Shows that the PID of the predecessor and successor are the same.
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * License: MIT
 */
//#include "../common.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	printf("Predecessor \"%s\": PID is %d\n", argv[0], getpid());
	printf("%s: will exec \"ps\" now..\n", argv[0]);
	/*
	 * gcc gives: envp.c:42: warning: missing sentinel in function call
	 * Solution:
	 * From the man page:
	 * "The list of arguments must be terminated by a NULL pointer, and,
	 * since these are variadic functions, this pointer must be
	 * cast (char *) NULL"
	 */
	if (execl("/bin/ps", "ps", (char *)0) == -1) {
		perror("execl failed");
		exit(1);
	}
	// this line will never be reached on execl() success
	exit(0);		// just to get rid of compiler warning
}
