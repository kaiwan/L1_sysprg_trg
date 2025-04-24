/* 
 * main_dies.c
 * From the LLNL Pthreads Tutorial
 *  Modified to have main() die before the other threads.. (-kaiwan).
 */
#define _POSIX_C_SOURCE    200809L	/* or earlier: 199506L */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS     5

void *PrintHello(void *threadid)
{
	printf("Thread %ld: Hello, world.\n", (long)threadid);
	pause();		// block on any signal
	pthread_exit(NULL);
}

int main(void)
{
	pthread_t threads[NUM_THREADS];
	long t;

	for (t = 0; t < NUM_THREADS; t++) {
		int rc;

		printf("main: Creating thread %ld\n", t);
		rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
		if (rc) {
			printf
			    ("ERROR; return code from pthread_create() is %d\n",
			     rc);
			exit(-1);
		}
	}
	sleep(10);
	printf("main: now exiting with pthread_exit...\n");
	pthread_exit(NULL);	/* The pthread_exit() causes this, the 'main' thread, to die
				   * and become a zombie! -as there are >=1 other (child) threads still alive...
				   */
}
