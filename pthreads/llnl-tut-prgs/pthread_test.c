/*
==============================================================================
pthread_test.c
C Code for pthread_create() test
==============================================================================
*/
#define _POSIX_C_SOURCE    200809L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NTHREADS 50000

void *do_nothing(void *null)
{
	int i;
	i = 0;
//	pause();
	pthread_exit(NULL);
}

int main()
{
	int rc, j;
	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (j = 0; j < NTHREADS; j++) {
		rc = pthread_create(&tid, &attr, do_nothing, NULL);
		if (rc) {
			printf
			    ("ERROR; return code from pthread_create() is %d\n",
			     rc);
			exit(1);
		}

		/* Wait for the thread */
		rc = pthread_join(tid, NULL);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n",
			       rc);
			exit(1);
		}
	}

	pthread_attr_destroy(&attr);
	pthread_exit(NULL);
}
