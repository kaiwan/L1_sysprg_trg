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

#define DEBUG  // hard-coded; conditionally put it in the Makefile (soft-code it)
//#undef DEBUG  // in production

static void catcher(int signo)
{
	printf("HIIIIIIIIIIII bad boy\n");
	char *p = malloc(50000); // wrong !
	(void)sleep(1);
	free(p);
}

int main()
{
	struct sigaction act;

	memset(&act, 0, sizeof(act));
	act.sa_handler = catcher;
#if 0
	sigemptyset(&act.sa_mask);	/* allow all signals while in the handler 
					 * (with the exception of the signal being handled) */
#else
	sigfillset(&act.sa_mask);	/* block all signals while in the handler
					 * (with the inclusion of the signal being handled)
		sigaddset() : selectively allow 1 signal
		sigdelset() : selectively clear 1 signal
	 */
#endif
	act.sa_flags = 0;	/* the sigemptyset() and flags init to 0 are redundant 
				 * due to the memset(), but we're just showing proper form */
#if 1
	if (sigaction(SIGINT, &act, 0) == -1) {
		perror("sigaction failure");
		exit(1);
	}
	if (sigaction(SIGQUIT, &act, 0) == -1) {
		perror("sigaction failure");
		exit(1);
	}
#endif
	for (;;)
		printf("\nExecuting for loop..");
}
