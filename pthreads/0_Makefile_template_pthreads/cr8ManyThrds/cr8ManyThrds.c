/*
 * cr8ManyThrds.c
 * (c) Kaiwan NB, kaiwanTECH
 * License: MIT
 */
#define _POSIX_C_SOURCE    200809L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_THREADS     50000	// arbitrary
typedef unsigned int u32;
typedef unsigned long u64;

char gbuf[100];

void *PrintStuff(void *threadnum)
{
	printf("Thread %05ld\n", (long)threadnum);
	pause();
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
