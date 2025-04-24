/*
 stacksz.c
 
 Testing minimal stack size that can be set for a thread.

 For N=100 (=> ~ 10000 bytes used on stack), empirically found (on my system at 
 least), that it "works" to a minimum stack size of 97 KB!

 Kaiwan NB.
 */
#define _POSIX_C_SOURCE    200809L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "convenient.h"

//#define DEBUG
#define NTHREADS 2
typedef long unsigned int u64;

static int N = 100;
pthread_attr_t attr;

static void foo(int a, int b)
{
	QP;
}

static void *dowork(void *tag)
{
	double arr[N][N]; // stress the stack by alloc'ing a large 10000*8 byte 2D array
	int i, j;
	long thrd = (long)tag;

	/* If enough stack memory is provided, all will be fine here.
	 * If *not*, *any* function being called below will fail and
	 * cause a segfault!
	 */
	QP;
	printf("\nIn thread #%ld:\nsizeof(double) = %ld; so stack mem = %ld\n",
		thrd, sizeof(double), sizeof(double)*N*N);
	foo(1, 2);
	//pthread_attr_getstacksize(&attr, &mystacksize);
	//printf("Thread # %zu : stack size = %zu bytes \n", (u64)tag, mystacksize);

	// Do something with arr[][] so that compiler keeps it...
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			arr[i][j] = ((i * j) / 3.452) + (N - i);

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	int rc;
    long j;
	pthread_t tid;
	size_t stacksize = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s stacksize_in_KB\n", argv[0]);
		exit(1);
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_getstacksize(&attr, &stacksize);
	printf("platform default MAX stacksize = %zu (=%zu Kb =%zu Mb), setting max stacksz to %d Kb\n",
		stacksize, stacksize/1024, stacksize/(1024*1024), 
	    atoi(argv[1]));

	// TODO: use strtol() and check for overflow !
	stacksize = atoi(argv[1]) * 1024;
		// use better APIs in production; like strtoul() etc... check for IoF !
	if (pthread_attr_setstacksize(&attr, stacksize)) {
		printf("pthread_attr_setstacksize (%lu bytes) failed!\n", stacksize);
		//perror("pthread_attr_setstacksize() failed");
		exit(1);
		//perror("pthread_attr_setstacksize");
	}

	for (j = 0; j < NTHREADS; j++) {
		rc = pthread_create(&tid, &attr, dowork, (void *)j);
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
