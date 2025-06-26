/* tp3-matrixmul.c */
#define _POSIX_C_SOURCE    200809L	/* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include "libpk.h"		/* MSG */

/*   
The following structure contains the necessary information  
to allow the function "dotprod" to access its input data and 
place its output into the structure.  This structure is 
unchanged from the sequential version.
*/

typedef struct {
	double *a;
	double *b;
	double sum;
	int veclen;
} DOTDATA;

/* Define globally accessible variables and a mutex */

#define NUMTHRDS 4
#define VECLEN 100

DOTDATA dotstr;
pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;

/*
The function dotprod is activated when the thread is created.
All input to this routine is obtained from a structure 
of type DOTDATA and all output from this function is written into
this structure. The benefit of this approach is apparent for the 
multi-threaded program: when a thread is created we pass a single
argument to the activated function - typically this argument
is a thread number. All  the other information required by the 
function is accessed from the globally accessible structure. 
*/

void *dotprod(void *arg)
{

	/* Define and use local variables for convenience */
	int i, start, end, offset, len;
	double mysum, *x, *y;
	offset = (long)arg;

	MSG("Thread T%d\n", offset);

	/*
	 * NOTE:
	 * 1. We skip the error checking code of the mutex [un]lock for conciseness; pl check!
	 * 2. Why take the lock here (below)? As we're _reading_ the global and that can still
	 *    constitute a critical section, a data race! (admittedly, it's a bit pedantic here)
	 */
	pthread_mutex_lock(&mutexsum);
	len = dotstr.veclen;
	start = offset * len;
	end = start + len;
	x = dotstr.a;
	y = dotstr.b;
	pthread_mutex_unlock(&mutexsum);

	/*
	   Perform the dot product and assign result
	   to the appropriate variable in the structure. 
	 */
	mysum = 0;
	for (i = start; i < end; i++) {
		mysum += (x[i] * y[i]);
	}
	MSG("  T%d: start %3d end %3d, mysum = %6.2f\n",
	    offset, start, end, mysum);
	/*
	 * Critical Section: Lock a mutex prior to updating the value in the shared
	 * structure, and unlock it upon updating.
	 */
	pthread_mutex_lock(&mutexsum);
	dotstr.sum += mysum;
	pthread_mutex_unlock(&mutexsum);

	pthread_exit((void *)0);
}

/* 
The main program creates threads which do all the work and then 
print out result upon completion. Before creating the threads,
the input data is created. Since all threads update a shared structure, 
we need a mutex for mutual exclusion. The main thread needs to wait for
all threads to complete, it waits for each one of the threads. We specify
a thread attribute value that allow the main thread to join with the
threads it creates. Note also that we free up handles  when they are
no longer needed.
*/
int main(int argc, char *argv[])
{
	long i;
	double *a, *b;
	int status;
	pthread_attr_t attr;

	/* Assign storage and initialize values */
	a = (double *)malloc(NUMTHRDS * VECLEN * sizeof(double));
	b = (double *)malloc(NUMTHRDS * VECLEN * sizeof(double));

	for (i = 0; i < VECLEN * NUMTHRDS; i++) {
		a[i] = 1;
		b[i] = a[i];
	}

	dotstr.veclen = VECLEN;
	dotstr.a = a;
	dotstr.b = b;
	dotstr.sum = 0;

	pthread_mutex_init(&mutexsum, NULL);

	/* Create threads to perform the dotproduct  */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < NUMTHRDS; i++) {
		/* 
		 * Each thread works on a different set of data.
		 * The offset is specified by 'i'. The size of
		 * the data for each thread is indicated by VECLEN.
		 */
		if (pthread_create(&callThd[i], &attr, dotprod, (void *)i)) {
			fprintf(stderr,
				"%s: thread %ld creation failed, aborting..\n",
				argv[0], i);
		}
	}

	pthread_attr_destroy(&attr);

	/* Wait on the worker threads to die */
	for (i = 0; i < NUMTHRDS; i++) {
		pthread_join(callThd[i], (void **)&status);
	}

	/* After joining, print out the results and cleanup */
	printf("Sum =  %f \n", dotstr.sum);
	free(a);
	free(b);
	pthread_mutex_destroy(&mutexsum);
	pthread_exit(NULL);
}
