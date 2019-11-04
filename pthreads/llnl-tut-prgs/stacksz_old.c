/*
 * stacksz.c
 * Test pthread stack size management..
 * Originally written by LLNL.
 */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NTHREADS 4
static int N=1000;

static pthread_attr_t attr;
 
void *dowork(void *threadid)
{
   double A[N][N];
   int i,j;
   long tid;
   size_t mystacksize; //=500*1024;
	pthread_attr_t locattr;

   tid = (long)threadid;
//   pthread_attr_init(&locattr);
//   pthread_attr_setstacksize (&locattr, mystacksize);
   pthread_attr_getstacksize (&locattr, &mystacksize);
   printf("Thread %ld: stack size = %d bytes \n", tid, mystacksize);
#if 0
   for (i=0; i<N; i++)
     for (j=0; j<N; j++)
      A[i][j] = ((i*j)/3.452) + (N-i);
#endif
   pthread_exit(NULL);
}
 
int main(int argc, char *argv[])
{
   pthread_t threads[NTHREADS];
   size_t stacksize;
   int rc;
   long t;
   void * stackaddr = malloc(sizeof(long));
 
   pthread_attr_init(&attr);
   pthread_attr_getstacksize (&attr, &stacksize);
//   pthread_attr_getstackaddr (&attr, &stackaddr); // Deprecated! does not work.
//   printf("Default stack size = %d. Stack addr = 0x%08x\n", stacksize, 
//                  (unsigned int)stackaddr);

   stacksize = 1*1024*1024; // 1 MB only
#if 0
   N=100; // we're okay with this, as the local 2d array's size will become 10,000 bytes
#else
   N=1000; // we're NOT okay with this, as the local 2d array's size will now become 100,000 bytes
#endif
   printf("Amount of stack needed per thread = %d\n",stacksize);

#if 1
   pthread_attr_setstacksize (&attr, stacksize);
#endif
   printf("Creating threads with stack size = %d bytes\n",stacksize);

   for(t=0; t<NTHREADS; t++){
//      rc = pthread_create(&threads[t], &attr, dowork, (void *)t);
      rc = pthread_create(&threads[t], NULL /*&attr*/, dowork, (void *)&attr);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }
   printf("Created %ld threads.\n", t);
   pause();
   pthread_exit(NULL);
}

