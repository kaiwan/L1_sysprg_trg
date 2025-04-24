/* matrixmul-dbg.c
 *
 * Create an addn thread just to demo thread dbg w/ gdb...
 */
//#define _POSIX_C_SOURCE    200809L            /* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <errno.h>
//#include "convenient.h"	/* MSG, DELAY_LOOP */

/*   
The following structure contains the necessary information  
to allow the function "dotprod" to access its input data and 
place its output into the structure.  This structure is 
unchanged from the sequential version.
*/

typedef struct {
   double      *a;
   double      *b;
   double     sum; 
   int     veclen; 
} DOTDATA;

typedef long unsigned int u64;

/* Define globally accessible variables and a mutex */

#define NUMTHRDS 4
#define VECLEN 100
   
DOTDATA gDotstr; 
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


static inline int pthread_mutex_lock_wrapped(pthread_mutex_t *mutex)
{
	int ret;
	if ((ret = pthread_mutex_lock(mutex))) {
		fprintf(stderr, "Warning! pthread_mutex_lock() failed, err=%d\n", ret);
		if (errno)
			perror(" ");
	}
	if (ret == EOWNERDEAD)
		printf("owner died without unlocking\n");
	return ret;
}

void *dotprod(void *arg)
{

   /* Define and use local variables for convenience */
   int ret, i, start, end, len ;
   double mysum, *x, *y;
   u64 offset = (u64)arg;

   printf("Thread T%lu\n", offset);
        
   /* Critical section (lock/unlock) when accessing the global! */
   ret = pthread_mutex_lock_wrapped(&mutexsum);
   if (ret == EOWNERDEAD) {
	printf("pthread_mutex_lock() returned EOWNERDEAD\n");
        printf("Lets now make the mutex consistent\n");
        if (pthread_mutex_consistent(&mutexsum)) {
		fprintf(stderr, "Warning! pthread_mutex_consistent() failed\n");
		pthread_mutex_unlock(&mutexsum);
	} else
		printf("mutex now consistent!\n");
   }

   len = gDotstr.veclen;
   x = gDotstr.a;

   /* TEST 1:
    * Bug with the mutex:
    * Attempt to relock an already locked mutex!
    * As type is ERRORCHECK, it should be detected...
    */
   if (offset == 0)
	pthread_mutex_lock_wrapped(&mutexsum);
   /* Fails with:
    * Warning! pthread_mutex_lock() failed, err=35 
    * #define EDEADLK 35 // Resource deadlock would occur
    */

   y = gDotstr.b;
   pthread_mutex_unlock (&mutexsum);

   start = offset*len;
   end   = start + len;

   /*
   Perform the dot product and assign result
   to the appropriate variable in the structure. 
   */

   mysum = 0;
   for (i=start; i < end ; i++) {
      mysum += (x[i] * y[i]);
   }
   //MSG ("  T%lu: start %3d end %3d, mysum = %6.2f\n", offset, start, end, mysum);
   /*
    * Critical Section: Lock a mutex prior to updating the value in the shared
    * structure, and unlock it upon updating.
    */
   pthread_mutex_lock (&mutexsum);
   gDotstr.sum += mysum;
   /* TEST 2:
    * Bug with the mutex:
    * With the third worker thread (when arg is 2), lets deliberately die
    * without unlocking the mutex!
    * As the 'robustness' attributes' set, it should be detected on the next lock attempt...
    */
   if (offset != 2)
	pthread_mutex_unlock(&mutexsum);
    /*
     * Output:
	Thread T3
	Warning! pthread_mutex_lock() failed, err=130
	owner died without unlocking
	pthread_mutex_lock() returned EOWNERDEAD
	Lets now make the mutex consistent
	mutex now consistent!

	  #define EOWNERDEAD 130 // Owner died
          #define ENOTRECOVERABLE 131 // State not recoverable
     */
   pthread_exit((void*) 0);
}

#if 0
void * do_work(void * msg)
{
	MSG("In do_work\n");
	DELAY_LOOP('W', 200);
	pthread_exit(NULL);
}
#endif

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
int main (int argc, char *argv[])
{
	double *a, *b;
	int status;
	u64 i;
	pthread_attr_t attr;
	pthread_mutexattr_t mutexattr;
	int ret, mtxtype;

#ifdef _POSIX_THREAD_PROCESS_SHARED
	printf("%s: mutex process-shared is true.\n", argv[0]);
#else
	printf("%s: mutex process-shared is false.\n", argv[0]);
#endif

 	/* Assign storage and initialize values */
 	a = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
 	b = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
  
   	for (i=0; i < VECLEN*NUMTHRDS; i++) {
     		a[i]=1;
     		b[i]=a[i];
   	}

   	gDotstr.veclen = VECLEN; 
   	gDotstr.a = a; 
   	gDotstr.b = b; 
   	gDotstr.sum=0;

	//--- Pthread mutex attributes
	// int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr,
	//  int *restrict type);
	if ((ret = pthread_mutexattr_gettype(&mutexattr, &mtxtype)))
		fprintf(stderr, "Warning! pthread_mutexattr_gettype() failed, err=%d\n", ret);
	printf("Mutex type is ");
	switch (mtxtype) {
	case PTHREAD_MUTEX_NORMAL: printf("DEFAULT (or NORMAL)\n"); break;
	case PTHREAD_MUTEX_ERRORCHECK: printf("ERRORCHECK\n"); break;
	case PTHREAD_MUTEX_RECURSIVE: printf("RECURSIVE\n"); break;
	//case PTHREAD_MUTEX_DEFAULT: printf("DEFAULT\n"); break;
	default: printf("-unknown-/error\n");
	}

	// int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
	if ((ret = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK)))
		fprintf(stderr, "Warning! pthread_mutexattr_settype() failed, err=%d\n", ret);

	// Set 'robustness'
	// int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robustness);
	if ((ret = pthread_mutexattr_setrobust(&mutexattr, PTHREAD_MUTEX_ROBUST)))
		fprintf(stderr, "Warning! pthread_mutexattr_settype() failed, err=%d\n", ret);

   	pthread_mutex_init(&mutexsum, &mutexattr);
	printf("(Provided no warnings occured) Mutex type is now PTHREAD_MUTEX_ERRORCHECK and robust\n");
	//---

         
   	/* Create threads to perform the dotproduct  */
   	pthread_attr_init(&attr);
   	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

#if 0
	// An extra work thread...
	if (pthread_create( &newthrd, &attr, do_work, NULL)) {
		fprintf(stderr, "%s: thread newthrd creation failed, aborting..\n", 
				argv[0]);
	}
#endif

	for (i=0; i<NUMTHRDS; i++) {
		/* 
		 * Each thread works on a different set of data.
		 * The offset is specified by 'i'. The size of
		 * the data for each thread is indicated by VECLEN.
		 */
		if (pthread_create( &callThd[i], &attr, dotprod, (void *)i)) {
			fprintf(stderr, 
			  "%s: thread %lu creation failed, aborting..\n", 
				argv[0], i);
		}
	}

 	pthread_attr_destroy(&attr);

    /* Wait on the other threads */
	for(i=0;i < NUMTHRDS;i++) {
		pthread_join( callThd[i], (void **)&status);
	}

   	/* After joining, print out the results and cleanup */
   	printf ("\nSum =  %f \n", gDotstr.sum);
   	free (a);
   	free (b);
   	pthread_mutex_destroy(&mutexsum);
   	pthread_exit(NULL);
}
