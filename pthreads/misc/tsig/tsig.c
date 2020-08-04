/*
 * tsig.c
 * Demo of signal handling in multi-threaded apps
 * 
 * Strategy:
 *  - mask (block) all signals in the main thread
 *  - now any thread created by main inherit it's signal mask, which 
 *    means that all signals will be blocked out in all subsequently 
 *    created threads;
 *  - create a separate signal handling thread that only catches all 
 *    required signals & handles them; it catches signals by calling sigwait().
 *
 * Also note the pretty rigorous error checking (essential of course, in 
 * any production software).
 * 
 * Kaiwan N Billimoria
 * <kaiwan -at- kaiwantech -dot- com>
 * License: MIT
 */
#define _POSIX_C_SOURCE    200112L	/* or earlier: 199506L */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#define	NUM_THREADS	2
#define FATAL		1
#define NON_FATAL	0

static inline void beep(int what)
{
	char buf[2];
	buf[0] = (char)(what);
	buf[1] = '\0';
	if (write(STDOUT_FILENO, buf, 1) < 0)
		perror("write fail");
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

#define MTX_LOCK(mutex, fatal)	do {				\
	int err;							\
	if ((err = pthread_mutex_lock (mutex))) {				\
		fprintf (stderr, "pthread_mutex_lock failed! : %s\n", strerror (err));	  \
		/* any clean up can go here ... */			\
		if (fatal) {						\
			fprintf (stderr, "Aborting now...\n");		\
			exit(1);					\
		}							\
		else { 						\
			fprintf (stderr, "*WARNING* pthread_mutex_lock failed!\n");	\
		}									\
	}										\
} while (0)

#define MTX_UNLOCK(mutex, fatal)	do {		\
	int err;							\
	if ((err = pthread_mutex_unlock (mutex))) {					\
		fprintf (stderr, "pthread_mutex_unlock failed! : %s\n", strerror (err));		\
		/* any clean up can go here ... */					\
		if (fatal) {								\
			fprintf (stderr, "Aborting now...\n");				\
			exit (1);							\
		}									\
		else {									\
			fprintf (stderr, "*WARNING* pthread_mutex_unlock failed!\n");	\
		}									\
	}										\
} while (0)

static int signal_handled = -1;
static pthread_mutex_t sig_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * signal_handler() is the thread that handles all signal catching
 * on behalf of all the other threads; it does this by waiting for signals 
 * on sigwait().
 *
 * This is fine for the "normal" POSIX thread/signal handling but poses a 
 * special case on Linux. This is as the LinuxThreads implementation does 
 * not have a concept of sending a signal to the overall "process". On Linux, 
 * every process is essentially a thread. So if one thread calls sigwait() & 
 * all other threads block all signals, only the signals specifically sent to 
 * the sigwait-ing thread will be processed. So, depending on your application, 
 * this could mean that on LinuxThreads, you have no choice but to install 
 * an asynchronous signal handler for each thread.
 * 
 */
static void *signal_handler(void *arg)
{
	sigset_t sigset;
	int sig;

	printf("Dedicated signal_handler() thread alive..\n");
	while (1) {
		/* Wait for any/all signals */
		if (sigfillset(&sigset) == -1) {
			perror("signal_handler: sigfillset failed");
			/* clean up */
			exit(1);
		}
		if (sigwait(&sigset, &sig) < 0) {
			perror("signal_handler: sigwait failed");
			/* clean up */
			exit(1);
		}
		/* Note on sigwait():
		 *    sigwait suspends the calling thread until one of 
		 *    (any of) the  signals  in set is delivered to the 
		 *    calling thread. It then stores the number of the 
		 *    signal received in the location  pointed  to  by  
		 *    "sig" and returns.  The  signals  in set must be 
		 *    blocked and not ignored on entrance to sigwait. 
		 *    If the delivered signal has a  signal handler 
		 *    function attached, that function is *not* called.
		 */

		/* We've caught a signal! 
		 * Here, as a demo, we're setting a global (hence the locking) to the 
		 * signal caught. In a 'real' app, handle the signal here itself..
		 */
		switch (sig) {
		case SIGINT:
			MTX_LOCK(&sig_mutex, FATAL);
			signal_handled = SIGINT;
			MTX_UNLOCK(&sig_mutex, FATAL);
			break;

		case SIGQUIT:
			MTX_LOCK(&sig_mutex, FATAL);
			signal_handled = SIGQUIT;
			MTX_UNLOCK(&sig_mutex, FATAL);
			break;

		case SIGIO:
			MTX_LOCK(&sig_mutex, FATAL);
			signal_handled = SIGIO;
			MTX_UNLOCK(&sig_mutex, FATAL);
			break;
			/* ... */
		default:
			MTX_LOCK(&sig_mutex, FATAL);
			signal_handled = sig;
			MTX_UNLOCK(&sig_mutex, FATAL);
			break;
		}		// switch
		printf("!!! signal_handler(): caught signal #%d !!!\n", sig);
	}			// while
	return (void *)0;
}

static void *work(void *id)
{
	long this = (long)id;
	printf("+++++++++++++++++++++++++++++++++++++++++\n\
Worker thread #%ld (pid %d)...\n", this, getpid());
	if (this == 1) {
		DELAY_LOOP('1', 2000);
	} else if (this == 2) {
		DELAY_LOOP('2', 2000);
	}
	printf("\n---------------------------------------\n");
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	sigset_t sigset;
	pthread_t pthrd[NUM_THREADS + 1];
	pthread_attr_t attr;
	long t = 0;

	/* 
	 * Block *all* signals here in the main thread.
	 * Now all subsequently created threads also block all signals.
	 */
	sigfillset(&sigset);
	if (pthread_sigmask(SIG_BLOCK, &sigset, NULL)) {
		perror("main: pthread_sigmask failed");
		/* clean up */
		exit(1);
	}

	if (pthread_attr_init(&attr)) {
		perror("main: pthread_attr_init failed");
		/* clean up */
		exit(1);
	}
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
		perror("main: pthread_attr_setdetachstate failed");
		/* clean up */
		exit(1);
	}

	/* Create the dedicated signal handling thread */
	if (pthread_create(&pthrd[t], &attr, signal_handler, NULL)) {
		fprintf(stderr, "%s: thread %ld creation failure..\n", argv[0],
			t);
		perror("main: pthread_create");
		/* clean up */
		exit(1);
	}

	/* Create worker threads */
	for (t = 1; t < NUM_THREADS + 1; t++) {
		if (pthread_create(&pthrd[t], &attr, work, (void *)t)) {
			fprintf(stderr, "%s: thread %ld creation failure..\n",
				argv[0], t);
			perror("main: pthread_create");
			/* clean up */
			exit(1);
		}
	}

	/* Block on signals forever; until we catch a fatal one! */
	while(1)
		pause();
	/* this code never reached */
	pthread_exit(NULL);
}
/* end tsig.c */
