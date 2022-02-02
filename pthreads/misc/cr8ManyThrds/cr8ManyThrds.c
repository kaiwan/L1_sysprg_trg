/*
 * cr8ManyThrds.c
 *
 * Write a Pthreads MT (multithreaded) app to spawn threads; the number of
 * threads to spawn is to be given as a command-line parameter. You must
 * have a validity check on the maximum number of threads to create, based on
 * both the resource limit RLIMIT_NPROC (use the prlimit(2) API to query it)
 * and the overall system limit.
 *
 * (c) Kaiwan NB, kaiwanTECH
 * License: MIT
 */
#define _POSIX_C_SOURCE    200112L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_THREADS    127000 // 50000	// arbitrary
typedef unsigned int u32;
typedef unsigned long u64;

char gbuf[100];

void *PrintStuff(void *threadnum)
{
	printf("Thread %05ld\n", (long)threadnum);
#if 1
	// we want the worker threads alive...
	pause();
#else
	/* Detaching them allows creation of more threads IFF we allow the
     * workers to die! */
	pthread_detach(pthread_self());
#endif
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	pthread_t *threads;
	int numthrds;
	long t;

	if (argc != 2) {
		printf("Usage: %s num-threads-to-create\n", argv[0]);
		exit(1);
	}
	numthrds = atoi(argv[1]);
	if ((numthrds <= 0) || (numthrds > MAX_THREADS)) {
		printf("%s: num-threads invalid\n", argv[0]);
		exit(1);
	}

	threads = malloc(sizeof(pthread_t) * numthrds);
	if (!threads) {
		printf("%s: out of memory!\n", argv[0]);
		exit(1);
	}

	for (t = 0; t < numthrds; t++) {
		int rc;
		// TODO : Assumption: they're joinable...
		rc = pthread_create(&threads[t], NULL, PrintStuff, (void *)t);
		if (rc) {
			printf
			    ("%s: Thrd # %ld: ERROR: return code from pthread_create() is %d\n",
			     argv[0], t, rc);
			perror("pthread_create failed");
			exit(1);
		}
	}
	pause();
	free(threads);
	pthread_exit(NULL);
}
