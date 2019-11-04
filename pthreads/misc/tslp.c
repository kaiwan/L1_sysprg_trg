/*
 * tslp.c
 *  Sleeping in a thread- a thread(s) can sleep; other "live" threads 
 *  continue to execute, even if main() dies.
 *
 * gcc tslp.c -o tslp -Wall -lpthread
 */
#define _POSIX_C_SOURCE    200112L	/* or earlier: 199506L */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <asm/param.h>		// for HZ =100 by default
#include "libpk.h"		// misc common routines

#define	PRINT_CHAR		0
#define	AUDIBLE_BEEP	1

void *thrd_p2(void *);

int main(int argc, char **argv)
{
	pthread_t p2;
	int i = 0, r;

	printf("p1 (pid %d - the main() thread): now creating pthread p2..\n",
	       getpid());
	r = pthread_create(&p2,	// thread id
			   NULL,	// thread attributes (use default)
			   thrd_p2,	// function to execute
			   (void *)i);	// argument to function
	if (r)
		perror("pthread creation"), exit(1);

	printf("main thread doing delay loop now..\n");
	DELAY_LOOP('7', 100);
	printf("main thread exiting..\n");

	pthread_exit(NULL);
	printf("2 main thread exiting..\n");
	// never reached
}

void *thrd_p2(void *msg)
{
	printf("  Thread p2 here in function thrd_p2\n");
	puts("  p2: sleeping for 50s..");
	sleep(50);

	puts("  p2: exiting..");
	pthread_exit("p2 joined");
}

/* Output:
*
* $ 
* $ gcc -Wall tslp.c -o tslp -lpthread
* $ 
$ ./tslp 
p1 (pid 6461 - the main() thread): now creating pthread p2..
main thread doing delay loop now..
7  Thread p2 (pid 6463) here in function thrd_p2
p2: sleeping for 10s..
777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777  p2: exiting..
777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777main thread exiting..
$ 
*/
