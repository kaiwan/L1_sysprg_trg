/*
 * tmslc.c
 * Print out timeslice.
 * Works only for tasks running w/ the SCHED_RR scheduling policy..
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>

int main(int argc, char **argv)
{
	struct timespec tp;
	int policy, ret;
	pid_t pid = 0;

	if (argc == 1) {
		fprintf(stderr, "Usage: %s pid\n", argv[0]);
		exit(1);
	}
	pid = atoi(argv[1]);

	/*
	   Query the sched policy: POSIX says that sched_rr_get_interval() works only for SCHED_RR tasks.
	   HOWEVER, on Linux it always works.
	   BUT, with the new(er) 2.6 CFS scheduler, there is *no* timeslice!
	 */
#if 1
	if ((policy = sched_getscheduler(pid)) == -1) {
		perror("sched_getscheduler");
		exit(1);
	}
	printf("policy = %d\n", policy);
	if (policy != SCHED_RR) {
		printf
		    ("%s: sched policy of PID %d is not SCHED_RR, cannot query timeslice!\n",
		     argv[0], pid);
		return 1;
	}
#endif

	/* get the current task’s timeslice length */
	ret = sched_rr_get_interval(pid, &tp);
	if (ret == -1) {
		perror("sched_rr_get_interval");
		return 1;
	}

	/* convert the seconds and nanoseconds to milliseconds */
	printf("Our time quantum is %.2lf milliseconds\n",
	       (tp.tv_sec * 1000.0f) + (tp.tv_nsec / 1000000.0f));
	return 0;
}
