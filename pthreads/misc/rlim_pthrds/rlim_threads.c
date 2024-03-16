/*
 * rlim_threads.c
 *  We want to answer the question, "within a multithreaded app, do all
 *  threads share the resource limits?"
 *
 * Ans.
 * a) [older] Find that, no, every thread has it's own (private) set of
 * resource limits.
 * b) [newer, Feb'11, Ubuntu 10.10, kernel ver 2.6.35-25-generic]
 * YES, all threads do seem to *share* / inherit the resource limits!
 *
 *
 * Compile:
 * gcc rlim_threads.c -o rlim_threads -Wall -pthread
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * License: Dual MIT / GPL
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/resource.h>

#define DELAY	2

/* Query rlimit */
static int query_rlimit(int limit, struct rlimit *rlim)
{
	if (getrlimit(RLIMIT_NOFILE, rlim) == -1) {
		perror("getrlimit failed");
		return -1;
	}
	return 0;
}

void *thrd_p2(void *msg)
{
	struct rlimit rlim2;

	/* Note that the PID displayed here will be the same as the parent
	 * thread's as the getpid() syscall now returns the TGID
	 */
	printf("  %s(): sleeping for %d s..\n", __func__, DELAY);
	sleep(DELAY);

	query_rlimit(RLIMIT_NOFILE, &rlim2);
	printf("  %s(): RLIMIT_NOFILE currently set to:\n"
	       "\tSoft limit : %d\n"
	       "\tHard limit : %d\n", __func__, (int)rlim2.rlim_cur, (int)rlim2.rlim_max);

	printf("  %s(): now sleeping for 5s..\n", __func__);
	sleep(5);

	query_rlimit(RLIMIT_NOFILE, &rlim2);
	printf("  %s(): RLIMIT_NOFILE currently set to:\n"
	       "\tSoft limit : %d\n"
	       "\tHard limit : %d\n", __func__, (int)rlim2.rlim_cur, (int)rlim2.rlim_max);

	printf("  %s(): exiting..", __func__);
	pthread_exit("thrd_p2 joined");
}

int main(int argc, char **argv)
{
	pthread_t p2;
	long i = 0;
	int r;
	struct rlimit rlim;

	printf("p1 (pid %d - the main() thread): now creating pthread p2..\n",
	       getpid());
	r = pthread_create(&p2,	// thread id
			   NULL,	// thread attributes (use default)
			   thrd_p2,	// function to execute
			   (void *)i);	// argument to function
	if (r)
		perror("pthread creation"), exit(1);

	query_rlimit(RLIMIT_NOFILE, &rlim);
	printf("main: RLIMIT_NOFILE currently set to:\n"
	       "\tSoft limit : %d\n"
	       "\tHard limit : %d\n", (int)rlim.rlim_cur, (int)rlim.rlim_max);

	printf("main: now sleeping for 3s...\n");
	sleep(3);

	printf("main: now setting RLIMIT_NOFILE to 500\n");
	rlim.rlim_cur = 500;
	rlim.rlim_max = 800;
	if (setrlimit(RLIMIT_NOFILE, &rlim) == -1) {
		perror("main: setrlimit failed");
		return -1;
	}
	printf("main: RLIMIT_NOFILE currently set to:\n"
	       "\tSoft limit : %d\n"
	       "\tHard limit : %d\n", (int)rlim.rlim_cur, (int)rlim.rlim_max);

	printf("main: now sleeping for 5s...\n");
	sleep(5);
	printf("main thread exiting..\n");
	pthread_exit(NULL);
}

/*
Output:

[RECENT, Aug 2020  : x86_64 Ubuntu 20.04 LTS ]

rlim_pthrds $ ./rlim_threads
p1 (pid 179653 - the main() thread): now creating pthread p2..
main: RLIMIT_NOFILE currently set to:
	Soft limit : 1024
	Hard limit : 1048576
main: now sleeping for 3s...
  thrd_p2: sleeping for 2 s..
  thrd_p2: RLIMIT_NOFILE currently set to:
	Soft limit : 1024
	Hard limit : 1048576
  thrd_p2: now sleeping for 5s..
main: now setting RLIMIT_NOFILE to 500
main: RLIMIT_NOFILE currently set to:
	Soft limit : 500
	Hard limit : 800
main: now sleeping for 5s...
  thrd_p2: RLIMIT_NOFILE currently set to:
	Soft limit : 500
	Hard limit : 800
  thrd_p2: exiting..
main thread exiting..
rlim_pthrds $ 

[NEWER]
 * b) [newer, Feb'11, Ubuntu 10.10, kernel ver 2.6.35-25-generic]

./rlim_threads 
p1 (pid 20468 - the main() thread): now creating pthread p2..
main: RLIMIT_NOFILE currently set to:
	Soft limit : 1024
	Hard limit : 1024
main: now sleeping for 3s...
  thrd_p2: sleeping for 2 s..
  thrd_p2: RLIMIT_NOFILE currently set to:
	Soft limit : 1024
	Hard limit : 1024
  thrd_p2: now sleeping for 5s..
main: now setting RLIMIT_NOFILE to 500
main: RLIMIT_NOFILE currently set to:
	Soft limit : 500
	Hard limit : 800
main: now sleeping for 5s...
  thrd_p2: RLIMIT_NOFILE currently set to:
	Soft limit : 500
	Hard limit : 800
  thrd_p2: exiting..
main thread exiting..
$ 

[OLDER]

$ ./rlim_threads
p1 (pid 24294 - the main() thread): now creating pthread p2..
main: RLIMIT_NOFILE currently set to:
        Soft limit : 1024
        Hard limit : 1024
main: now sleeping for 3s...
  thrd_p2: sleeping for 2 s..
  thrd_p2: RLIMIT_NOFILE currently set to:
        Soft limit : 1024
        Hard limit : 1024
  thrd_p2: now sleeping for 5s..
main: now setting RLIMIT_NOFILE to 500
main: RLIMIT_NOFILE currently set to:
        Soft limit : 500
        Hard limit : 500
main: now sleeping for 5s...
  thrd_p2: RLIMIT_NOFILE currently set to:
        Soft limit : 1024
        Hard limit : 1024
  thrd_p2: exiting..
main thread exiting..
$

*/
