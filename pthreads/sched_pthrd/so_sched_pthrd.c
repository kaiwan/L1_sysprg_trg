/*
 * sched_pthrd.c
 *
 * Build with:
 * gcc so_sched_pthrd.c -D_REENTRANT -Wall -lpthread -o so_sched_pthrd
 * 
 * Kaiwan NB.
 * [L]GPL
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <asm/param.h>

#define DEBUG	1		// 0 to switch off debug messages
#ifdef DEBUG
#define MSG(string, args...) fprintf(stderr, "%s:%s : " string, __FILE__, __FUNCTION__, ##args)
#else
#define MSG(string, args...)
#endif

static inline void beep(int what)
{
	char buf[2];
	buf[0] = (char)(what);
	buf[1] = '\0';
	write(STDOUT_FILENO, buf, 1);
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
                for(inner_index=0;inner_index<HZ*100000;inner_index++) \
                        x=(inner_index%2)*((22/7)%3); \
        } \
        /*printf("c=%d\n",c);*/\
}

void *thrd_p2(void *msg)
{
	struct sched_param p;
	/* The structure used is defined in linux/sched.h as:
	 * struct sched_param {
	 *      int sched_priority;
	 * };
	 */

	printf("  RT Thread p2 (LWP %d) here in function thrd_p2\n\
   setting sched policy to SCHED_FIFO and priority to %d in 2 seconds..\n", getpid(), (int)msg);
	sleep(2);

	/* pthread_setschedparam(3) internally becomes the syscall
	 * sched_setscheduler(2).
	 */
	p.sched_priority = (int)msg;
	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &p))
		perror("pthread_setschedparam");

	puts("  p2: working");
	DELAY_LOOP('2', 100);

	puts("  p2: exiting..");
	pthread_exit(NULL);
}

void *thrd_p3(void *msg)
{
	struct sched_param p;
	/* The structure used is defined in linux/sched.h as:
	 * struct sched_param {
	 *      int sched_priority;
	 * };
	 */
	int pri = (int)msg;

	pri += 10;
	printf("  RT Thread p3 (LWP %d) here in function thrd_p3\n\
   setting sched policy to SCHED_FIFO and priority HIGHER to %d in 4 seconds..\n", getpid(), pri);

	/* pthread_setschedparam(3) internally becomes the syscall
	 * sched_setscheduler(2).
	 */
	p.sched_priority = pri;
	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &p))
		perror("pthread_setschedparam");
	sleep(4);

	puts("  p3: working");
	DELAY_LOOP('3', 110);

	puts("  p3: exiting..");
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	pthread_t p2, p3;
	int rt_pri = 1, r;

	if (argc == 1)
		fprintf(stderr, "Usage: %s realtime-priority\n",
			argv[0]), exit(1);
	rt_pri = atoi(argv[1]);

	printf("main thread (%d): now creating realtime pthread p2..\n",
	       getpid());
	r = pthread_create(&p2, NULL,	// thread attributes (use default)
			   thrd_p2, (void *)rt_pri);
	if (r)
		perror("pthread creation"), exit(1);

	printf("main thread (%d): now creating realtime pthread p3..\n",
	       getpid());
	r = pthread_create(&p3, NULL,	// thread attributes (use default)
			   thrd_p3, (void *)rt_pri);
	if (r)
		perror("pthread creation"), exit(1);

	DELAY_LOOP('m', 200);
	pthread_exit(NULL);
}
