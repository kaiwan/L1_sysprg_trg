/* main_dtchd_dies.c 

   Modified (from main_dies) to have the worker threads be detached. main() dies
   before the other threads..  the effect is the same: the main thread becomes a zombie.
   (-kaiwan).
*/
#define _POSIX_C_SOURCE    200112L            /* or earlier: 199506L */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS     5

void *PrintHello(void *threadid)
{
   printf("Thread %d: Hello, world.\n", (int)threadid);
   pause(); // block on any signal
   pthread_exit(NULL);
}

int main()
{
   pthread_t threads[NUM_THREADS];
   pthread_attr_t attr;
   int rc, t;

    /* Initialize and set thread detached attribute */
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

   for(t=0;t < NUM_THREADS;t++){
      printf("main: Creating thread %d\n", t);
      rc = pthread_create(&threads[t], &attr, PrintHello, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

   pthread_attr_destroy(&attr);
   sleep(10);
   printf("main: now exiting with pthread_exit...\n");
   pthread_exit(NULL); /* The pthread_exit() causes this, the 'main' thread, to die 
           and become a zombie! -as there are >=1 other (child) threads still alive...
           */
}
