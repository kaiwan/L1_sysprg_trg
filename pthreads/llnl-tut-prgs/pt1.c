/* pt1.c 
   From the LLNL Pthreads Tutorial
*/
#define _POSIX_C_SOURCE    200112L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS     5

void *PrintHello(void *threadid)
{
	printf("%d: Hello World!\n", (int)threadid);
	pthread_exit(NULL);
}

int main()
{
	pthread_t threads[NUM_THREADS];
	int rc, t;

	for (t = 0; t < NUM_THREADS; t++) {
		printf("Creating thread %d\n", t);
		rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
		if (rc) {
			printf
			    ("ERROR; return code from pthread_create() is %d\n",
			     rc);
			exit(-1);
		}
	}
	pthread_exit(NULL);
}
