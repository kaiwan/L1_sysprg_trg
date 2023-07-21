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
#if 0
	printf("** Ouch! Received SIGINT. **\n");
		// why?? ...it will be covered! :-)
#else
	printf("%s():signo = %d\n", __func__, signo);
	// ssize_t write(int fd, const void *buf, size_t count);
	if (write(STDOUT_FILENO, "** Ouch! Received a signal. **", 30) == -1) {
		perror("sig1: write() failed");
		exit(1);
	}
#endif
	(void)sleep(1);
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
					 * (with the exception of the signal being handled)
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
