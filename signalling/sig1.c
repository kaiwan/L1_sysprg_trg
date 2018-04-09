/* sig1.c
 * Simple sigaction prg.
 * 
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * License: MIT.
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

static void catcher(int signo)
{
	if (write(STDOUT_FILENO, "** Ouch! Received SIGINT. **", 28) == -1) {
		perror("sig1: write() failed");
		exit(1);
	}
	(void)sleep(1);
}

int main()
{
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = catcher;
	sigemptyset(&act.sa_mask);	/* allow all signals while in the handler 
					 * (with the execption of the signal being handled) */
	act.sa_flags = 0;	/* the sigemptyset() and flags init to 0 are redundant 
				 * due to the memset(), but we're just showing proper form */
#if 1
	if (sigaction(SIGINT, &act, 0) == -1) {
		perror("sigaction failure");
		exit(1);
	}
#endif
	for (;;)
		printf("\nExecuting for loop..");
}
