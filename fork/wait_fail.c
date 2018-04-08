/*
 * wait_fail.c
 *
 * What's returned when we do a wait() with no
 * child?
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char **argv)
{
	printf("Issuing wait()...\n");
	if (wait(0) == -1) {
		printf("wait ret -1; errno = %d\n", errno);
		perror("wait failed");
		exit(1);
	}
	exit(0);
}

/*
Output:
$ ./wait_fail
Issuing wait()...
wait ret -1; errno = 10
wait failed: No child processes
$
*/
// end
