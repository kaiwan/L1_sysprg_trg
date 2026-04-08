/* pt2-join.c */
#define _POSIX_C_SOURCE    200809L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS	3

void *BusyWork(void *n)
{
	int i;
	double result = 0.0;

	for (i = 0; i < 1000000; i++) {
		result = result + 1;
		//result = result + (double)random();
	}
	printf("%s(): worker %ld: result = %f\n", __func__, (long)n, result);
	pthread_exit((void *)0);
}

int main()
{
	pthread_t work_thrd[NUM_THREADS];
	pthread_attr_t attr;
	int rc, status;
	long t;

	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	printf("NUM_THREADS = %d\n", NUM_THREADS);
	/* Creation loop */
	for (t = 0; t < NUM_THREADS; t++) {
		printf("%s(): Creating thread %ld\n", __func__, t);
		rc = pthread_create(&work_thrd[t], &attr, BusyWork, (void *)t);
		if (rc) {
			printf
			    ("ERROR; return code from pthread_create() is %d\n",
			     rc);
			exit(EXIT_FAILURE);
		}
	}

	/* Free attribute structure and wait for the other threads */
	pthread_attr_destroy(&attr);

	/* Join loop */
	for (t = 0; t < NUM_THREADS; t++) {
		printf("%s(): join loop # %ld\n", __func__, t);
		/* Note: "status" should not be local to the dying thread */
		rc = pthread_join(work_thrd[t], (void **)&status);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n",
			       rc);
			exit(EXIT_FAILURE);
		}
		printf("%s(): completed join with thread %ld status %d\n",
				__func__, t, status);
		//sleep(1);
	}
	/* By here, all (joinable) workers have died
	 * and been reaped by the main thread. 
	 * We can now have main die!
	 */
	pthread_exit(NULL);
}
