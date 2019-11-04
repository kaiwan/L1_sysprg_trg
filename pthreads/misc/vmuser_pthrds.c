/* 
vmuser_pthrds.c 
*/
#define _POSIX_C_SOURCE    200112L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS     5
typedef unsigned int u32;
typedef unsigned long u64;

char gbuf[100];

void *PrintStuff(void *threadnum)
{
	static int x = 5, y;
	int loc;

	if (__WORDSIZE == 32) {
		printf
		    ("Thread %d: &gbuf = %p ; statics &x = %p &y = %p; &loc = %p\n",
		     (int)threadnum, (u32) & gbuf, (u32) & x, (u32) & y,
		     (u32) & loc);
	} else if (__WORDSIZE == 64) {
		printf
		    ("Thread %d: &gbuf = %p ; statics &x = %p &y = %p; &loc = %p\n",
		     (int)threadnum, (u64) & gbuf, (u64) & x, (u64) & y,
		     (u64) & loc);
	}
	pthread_exit(NULL);
}

int main()
{
	pthread_t threads[NUM_THREADS];
	int rc, t;
	static int sm;

	if (__WORDSIZE == 32)
		printf("Thread main: &sm = %p ; loc &t = %p\n", (u32) & sm,
		       (u32) & t);
	else if (__WORDSIZE == 64)
		printf("Thread main: &sm = %p ; loc &t = %p\n", (u64) & sm,
		       (u64) & t);

	for (t = 0; t < NUM_THREADS; t++) {
		//printf("Creating thread %d\n", t);
		rc = pthread_create(&threads[t], NULL, PrintStuff, (void *)t);
		if (rc) {
			printf
			    ("ERROR; return code from pthread_create() is %d\n",
			     rc);
			exit(-1);
		}
	}
	pthread_exit(NULL);
}
