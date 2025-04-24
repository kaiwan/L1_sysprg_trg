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
#define _POSIX_C_SOURCE    200809L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_THREADS    1270000 // 50000	// arbitrary
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
		int ret;
		// TODO : Assumption: they're joinable...
		ret = pthread_create(&threads[t], NULL, PrintStuff, (void *)t);
		if (ret) {
			printf("%s: Thrd # %ld\n", argv[0], t);
			fprintf(stderr, "ERROR: return code from pthread_create() is %d\nstrerror() says:%s\n",
			     ret, strerror(ret));
			printf("err = %s\n", strerrorname_np(ret));
			//fprintf(stderr, "ERROR: return code from pthread_create() is %d\nstrerror() says:%s:%s\n",
			 //    ret, strerrorname_np(ret), strerror(ret));
			perror("perror(): pthread_create failed");
			exit(1);
		}
	}
//	pause();
	free(threads);
	pthread_exit(NULL);
}
