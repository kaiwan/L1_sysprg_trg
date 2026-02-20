/*
 * tsig.c
 * Demo of signal handling in multi-threaded apps
 * 
 * Strategy:
 *  - mask (block) all signals in the main thread
 *  - now any thread created by main inherit it's signal mask, which 
 *    means that all signals will be blocked out in all subsequently 
 *    created threads;
 *    EXCEPTION:
 *      Do NOT handle the kernel synchronous signals this way - the ones sent
 *      to the faulting thread on a fault/bug:
 *	 SIGSEGV / SIGBUS / SIGABRT [/SIGIOT] / SIGFPE / SIGILL
 *	For them, use the usual sigaction(2) style handling.
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
//#define _POSIX_C_SOURCE    200112L    /* or earlier: 199506L */
// causes issue w/ SA_RESTART etc ! Don't use it! Use this:
//#define _POSIX_C_SOURCE    200809L
// ref: https://stackoverflow.com/questions/9828733/sa-restart-not-defined-under-linux-compiles-fine-in-solaris

//#define _GNU_SOURCE // the Makefile defines this

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>		// gettid(2)
#include <string.h>
#include <errno.h>

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
static int g_opt;

/* 
 * Handler for the synchronous / fatal signals:
 * SIGSEGV / SIGBUS / SIGABRT / SIGFPE / SIGILL / SIGIOT
 */
static void fatal_sigs_handler(int signum, siginfo_t * siginfo, void *rest)
{
	static volatile sig_atomic_t c = 0;

	printf("\n*** %s(): [%d] PID %d", __func__, ++c, getpid());
#ifdef __linux__
	printf(" (TID %d)", gettid());
#endif
	printf("	received signal %d. errno=%d\n"
	       " Cause/Origin: (si_code=%d): ",
	       signum, siginfo->si_errno, siginfo->si_code);

	switch (siginfo->si_code) {
	case SI_USER:
		printf("user\n");
		break;
	case SI_KERNEL:
		printf("kernel\n");
		break;
	case SI_QUEUE:
		printf("queue\n");
		break;
	case SI_TIMER:
		printf("timer\n");
		break;
	case SI_MESGQ:
		printf("mesgq\n");
		break;
	case SI_ASYNCIO:
		printf("async io\n");
		break;
	case SI_SIGIO:
		printf("sigio\n");
		break;
	case SI_TKILL:
		printf("t[g]kill\n");
		break;
	// other poss values si_code can have for SIGSEGV 
	case SEGV_MAPERR:
		printf("SEGV_MAPERR: address not mapped to object\n");
		break; 
	case SEGV_ACCERR: 
		printf("SEGV_ACCERR: invalid permissions for mapped object\n"); 
		break;
/* 
 * Now it seems fine...
 * OLD: SEGV_BNDERR and SEGV_PKUERR result in compile failure ??  * Qs asked on SO here:
 *https://stackoverflow.com/questions/45229308/attempting-to-make-use-of-segv-bnderr-and-segv-pkuerr-in-a-sigsegv-signal-handle
*/
#if 1
	case SEGV_BNDERR:	/* 3.19 onward */
		printf("SEGV_BNDERR: failed address bound checks\n");
	case SEGV_PKUERR:	/* 4.6 onward */
		printf
		    ("SEGV_PKUERR: access denied by memory-protection keys\n");
#endif
	default:
		printf("-none-\n");
	}
	printf(" Faulting addr=%p\n", siginfo->si_addr);

	// Use the psiginfo() helper!
	psiginfo(siginfo, "siginfo_t details follow");

	/*
	 * Can reset signal action to default and raise it on ourself,
	 * to get the kernel to emit a core dump
	 */
	if (signal(SIGSEGV, SIG_DFL) == SIG_ERR)
		fprintf(stderr, "signal -reverting SIGSEGV to default- failed");
	if (raise(SIGSEGV))
		fprintf(stderr, "raise SIGSEGV failed");
	
#if 1
	exit(1);
#else
	abort();
#endif
}

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
		/*
		 * Can use the sigwaitinfo(2) (or even the sigtimedwait(2)) syscalls
		 * to get detailed info on what happened from the kernel !
		 */
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
		printf("!!! %s(): caught signal #%d !!!\n", __func__, sig);
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
		if (g_opt == 1) {
			int *pi = 0x0;
			printf("pi = %p\n", (void *)*pi);	// NULL-ptr dereference bug!
			/* BUG ! 
			 * Causes a fault at the level of the MMU (as all bytes in virtual page 0
			 * have no permission '---'); thus, causing the MMU to raise a fault, leading
			 * the OS's fault handling code to send SIGSEGV to the offending *thread*, the
			 * one that caused this to occur!
			 */
		}
	} else if (this == 2) {
		DELAY_LOOP('2', 2000);
	}
	printf("\n---------------------------------------\n");
	pthread_exit(NULL);
}

/**
 * setup_altsigstack - Helper function to set alternate stack for sig-handler
 * @stack_sz:	required stack size
 *
 * Return: 0 on success, -ve errno on failure
 */
int setup_altsigstack(size_t stack_sz)
{
	stack_t ss;

	printf("Alt signal stack size = %zu bytes\n", stack_sz);
	ss.ss_sp = malloc(stack_sz);
	if (!ss.ss_sp) {
		printf("malloc(%zu) for alt sig stack failed\n", stack_sz);
		return -ENOMEM;
	}

	ss.ss_size = stack_sz;
	ss.ss_flags = 0;
	if (sigaltstack(&ss, NULL) == -1) {
		printf("sigaltstack for size %zu failed!\n", stack_sz);
		return -errno;
	}
	printf("Alt signal stack uva (user virt addr) = %p\n", ss.ss_sp);

	return 0;
}

int main(int argc, char **argv)
{
	sigset_t sigset;	// used for signal mask
	struct sigaction act;
	pthread_t pthrd[NUM_THREADS + 1];
	pthread_attr_t attr;
	long t = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <option>\n\
	0 = do NOT cause a segfault\n\
	1 = do cause a segfault\n", argv[0]);
		exit(1);
	}
	g_opt = atoi(argv[1]);
	if (g_opt != 0 && g_opt != 1) {
		fprintf(stderr, "<option>: bad value passed\n");
		exit(1);
	}

	/* 
	 * Block all, well, most, signals here in the main thread.
	 * Now all subsequently created threads also block all signals.
	 */
	if (sigfillset(&sigset) < 0) {
		perror("sigfillset");
		exit(1);
	}
	/* Do NOT block the synchronous (or 'fatal') signals - the ones sent to the faulting thread
	 * on a fault/bug:
	 * SIGSEGV / SIGBUS / SIGABRT [/SIGIOT] / SIGFPE / SIGILL
	 */
	if (sigdelset(&sigset, SIGSEGV) < 0) {
		perror("sigdelset");
		exit(1);
	}
	if (sigdelset(&sigset, SIGBUS) < 0) {
		perror("sigdelset");
		exit(1);
	}
	if (sigdelset(&sigset, SIGABRT) < 0) {
		perror("sigdelset");
		exit(1);
	}
	if (sigdelset(&sigset, SIGFPE) < 0) {
		perror("sigdelset");
		exit(1);
	}
	if (sigdelset(&sigset, SIGILL) < 0) {
		perror("sigdelset");
		exit(1);
	}
	if (pthread_sigmask(SIG_BLOCK, &sigset, NULL)) {
		perror("main: pthread_sigmask failed");
		/* clean up */
		exit(1);
	}

	/* Handle synchronous signals - the SIGSEGV, etc. as a special case */
	/* Use a separate stack for signal handling via the SA_ONSTACK;
	 * This is critical, especially for handling the SIGSEGV; think on it, what
	 * if this process crashes due to stack overflow; then it will receive the
	 * SIGSEGV from the kernel (when it attempts to eat into unmapped memory
	 * following the end of the stack)! The SIGSEGV signal handler must now run
	 * But where? It cannot on the old stack - it's now corrupt! Hence, the
	 * need for an alternate signal stack !
	 */
	if (setup_altsigstack(10 * 1024 * 1024) < 0) {
		fprintf(stderr, "%s: setting up alt sig stack failed\n",
			argv[0]);
		exit(1);
	}
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = fatal_sigs_handler;
	act.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGSEGV, &act, 0) == -1) {
		perror("sigaction");
		exit(1);
	}
	if (sigaction(SIGBUS, &act, 0) == -1) {
		perror("sigaction");
		exit(1);
	}
	if (sigaction(SIGABRT, &act, 0) == -1) {
		perror("sigaction");
		exit(1);
	}
	if (sigaction(SIGFPE, &act, 0) == -1) {
		perror("sigaction");
		exit(1);
	}
	if (sigaction(SIGILL, &act, 0) == -1) {
		perror("sigaction");
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
	while (1)
		pause();
	/* this code never reached */
	pthread_exit(NULL);
}

/* end tsig.c */
