/*
 * sched_pthrd.c
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "libpk.h"

#define DEBUG	1		// 0 to switch off messages
#ifdef DEBUG
  	#define MSG(string, args...) fprintf(stderr, "%s:%s : " string, __FILE__, __FUNCTION__, ##args)
#else
	#define MSG(string, args...)
#endif

#define	TWO_RT_THREADS		1

/**************************************
 * Addn. Note from the article "Low Latency in the Linux Kernel" by Dave 
 * Phillips 
 * http://www.linuxdevcenter.com/lpt/a/467
 *
 * Low-latency in the real world: performance & programming examples
 *
 * The POSIX specification provides a "soft" real-time API with the SCHED_FIFO
 * macro.
 * Programmers can add real-time scheduling to their applications by adding a 
 * very small piece of code to raise the application's performance priority.
 * Benno Senoner has suggested this fragment:
 *
 * #include <sched.h> int set_real-time_priority(void)
 * {
 *     struct sched_param schp;
 *
 *     // set the process to real-time privs
 *     memset(&schp, 0, sizeof(schp));
 *     schp.sched_priority = sched_get_priority_max(SCHED_FIFO);
 *
 *     if (sched_setscheduler(0, SCHED_FIFO, &schp) != 0) {
 *             perror("sched_setscheduler");
 *             return -1;
 *     }
 *
 *      return 0;
 *
 * }
 *
 * Benno also advises programmers that "... A low latency kernel alone is not 
 * enough to get dropout-free real-time audio I/O: the application must be 
 * written in such a way that the audio thread never blocks (except when 
 * doing audio I/O), and all communication with the outside world must be 
 * done by using lock-free data structures like shared-mem and lock-free 
 * FIFOs."
 *
 *************************/

void * thrd_p2( void * msg )
{
	struct sched_param p;
	/* The structure used is defined in linux/sched.h as:
	* struct sched_param {
	* 	int sched_priority;
	* };
	*/

	printf("  RT Thread p2 (LWP %d) here in function thrd_p2\n\
   setting sched policy to SCHED_FIFO and priority to %d in 2 seconds..\n",
					getpid(), (int)msg );
	sleep(2);

	/* pthread_setschedparam(3) internally becomes the syscall
	 * sched_setscheduler(2).
	 */
	p.sched_priority=(int)msg;
	if( pthread_setschedparam( pthread_self(), SCHED_FIFO, &p ) )
		perror("pthread_setschedparam");
	
	puts("  p2: working");
	DELAY_LOOP('2', 300);

	puts("  p2: exiting..");
	pthread_exit(NULL);
}


void * thrd_p3( void * msg )
{
	struct sched_param p;
	/* The structure used is defined in linux/sched.h as:
	* struct sched_param {
	* 	int sched_priority;
	* };
	*/
	int pri=(int)msg;

	pri+=10;
	printf("  RT Thread p3 (LWP %d) here in function thrd_p3\n\
   setting sched policy to SCHED_FIFO and priority HIGHER to %d in 4 seconds..\n", 
					getpid(), pri);

	/* pthread_setschedparam(3) internally becomes the syscall
	 * sched_setscheduler(2).
	 */
	p.sched_priority=pri;
	if( pthread_setschedparam( pthread_self(), SCHED_FIFO, &p ) )
		perror("pthread_setschedparam");
	sleep(4);
	
	puts("  p3: working");
	DELAY_LOOP('3', 310);

	puts("  p3: exiting..");
	pthread_exit(NULL);
}


int main( int argc, char **argv )
{
	pthread_t p2, p3;
	int rt_pri=1, r, min, max;

	if( argc==1 )
		fprintf(stderr,"Usage: %s realtime-priority\n", 
			argv[0]), exit(1);
	min = sched_get_priority_min(SCHED_FIFO);
	if (min == -1) {
		perror("sched_get_priority_min failure");
		exit (1);
	}
	max = sched_get_priority_max(SCHED_FIFO);
	if (max == -1) {
		perror("sched_get_priority_max failure");
		exit (1);
	}
	MSG ("SCHED_FIFO priority range is %d to %d\n", min, max);
	
	rt_pri=atoi(argv[1]);
	if ((rt_pri < min) || (rt_pri > max)) {
		fprintf(stderr,"%s: Priority value passed (%d) out of range.\n",
			argv[0], rt_pri);
		exit (1);
	}
	
	printf("\nNote: to create true RT threads, you need to run this \
program as superuser\n");
	printf("main thread (%d): now creating realtime pthread p2..\n", 
		getpid());	
	r = pthread_create( &p2,	// thread id
			NULL,		// thread attributes (use default)
			thrd_p2,	// function to execute
			(void *)rt_pri );		// argument to function
	if( r ) perror( "pthread creation" ), exit(1);

	printf("main thread (%d): now creating realtime pthread p3..\n", 
		getpid());	
	r = pthread_create( &p3,	// thread id
			NULL,		// thread attributes (use default)
			thrd_p3,	// function to execute
			(void *)rt_pri );		// argument to function
	if( r ) perror( "pthread creation" ), exit(1);

	DELAY_LOOP('m', 600);
	pthread_exit( NULL );
}

// end sched_pthrd.c
