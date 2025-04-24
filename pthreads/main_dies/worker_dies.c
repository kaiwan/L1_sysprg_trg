/* worker_dies.c 
   Modified to have a worker thread die before the other threads..
   (-kaiwan).
*/
#define _POSIX_C_SOURCE    200809L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS     3

static inline void beep(int what)
{
	char buf[2];
	buf[0] = (char)(what);
	buf[1] = '\0';
	write(STDOUT_FILENO, buf, 1);
}

/* 
 * @val : ASCII value to print
 * @loop_count : times to loop around
 */
#define DELAY_LOOP(val,loop_count) \
{ \
    int c=0;\
    unsigned int for_index,inner_index; \
    double x; \
    for(for_index=0;for_index<loop_count;for_index++) { \
        beep((val)); \
        c++;\
        for(inner_index=0;inner_index<10000000;inner_index++) \
            x=(inner_index%2)*((22/7)%3); \
    } \
    /*printf("c=%d\n",c);*/\
}

void *PrintHello(void *threadid)
{
	long t = (long)threadid;

	printf("Thread %ld\n", t);
	switch (t) {
	case 0:		//DELAY_LOOP('0', 3000); printf("\n");
		/* Make the first worker thread take (much) longer than the others to finish.
		 * Thus, the main thread will be waiting to join it... so when the other 2 workers 
		 * expire earlier, will they become zombies??
		 * A. No, they do not. When the first worker finally expires, all 3 threads are joined.
		 */
		sleep(10);
		printf("Thread %ld exiting..\n", t);
		break;
	case 1:		//DELAY_LOOP('1', 50); printf("\n");
		sleep(3);
		printf("Thread %ld exiting..\n", t);
		break;
	case 2:		//DELAY_LOOP('2', 10); printf("\n");
		sleep(5);
		printf("Thread %ld exiting..\n", t);
		break;
	}
	pthread_exit(NULL);
}

int main()
{
	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	int rc, status;
	long t;

	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (t = 0; t < NUM_THREADS; t++) {
		printf("main: Creating thread %ld\n", t);
		rc = pthread_create(&threads[t], &attr, PrintHello, (void *)t);
		if (rc) {
			printf
			    ("ERROR; return code from pthread_create() is %d\n",
			     rc);
			exit(-1);
		}
	}

/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for (t = 0; t < NUM_THREADS; t++) {
		rc = pthread_join(threads[t], (void **)&status);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n",
			       rc);
			exit(1);
		}
		printf("Completed join with thread %ld status= %d\n", t,
		       status);
	}
	pthread_exit(NULL);
}
