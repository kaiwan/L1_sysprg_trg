/*
 * sender_mq.c

 Simple POSIX Message Queue Producer/Consumer demo.
 This program is the "producer" (or sender) app.

 Should mount the 'mqueue' filesystem as root:
 # mkdir /dev/mq
 # mount -t mqueue none /dev/mq

 Can then see that messages have been written...
 $ mount |grep mqueue
 none on /dev/mq type mqueue (rw)
 $ 
 
 $ l /dev/mq/
 total 0
 -rw-r--r-- 1 kaiwan kaiwan 80 May 28 12:15 myposixmq
 $ cat /dev/mq/myposixmq 
 QSIZE:30         NOTIFY:0     SIGNO:0     NOTIFY_PID:0
 $ 

 FIXME : 
  There's definitely a race here between sender and receiver; fix it 
  using the POSIX semaphore!

  (c) 2012, kaiwan billimoria, [L]GPL.
 */
#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define MQNAME "/myposixmq"

int main(int argc, char **argv)
{
	mqd_t mymq;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s \"msg to send\" priority\n", argv[0]);
		exit (1);
	}

	int len = strlen(argv[1]);
	if (len <= 0) {
		fprintf(stderr, "%s: invalid message? Aborting...\n", argv[0]);
		exit (1);
	}

	unsigned prio=atoi(argv[2]);
	if ((prio < 0) || (prio > sysconf(_SC_MQ_PRIO_MAX))) {
		fprintf(stderr, "%s: invalid priority value, aborting...\n", argv[0]);
		exit (1);
	}
	//printf("max prio: %ld\n", sysconf(_SC_MQ_PRIO_MAX));

	mymq = mq_open(MQNAME, O_RDWR|O_CREAT|O_EXCL, 0644, NULL); // 'attr' as NULL => use defaults
	if (-1 == mymq) {
		if (EEXIST == errno) { // failed as the MQ already exists!
			mymq = mq_open(MQNAME, O_RDWR);
			if (-1 == mymq)
				handle_error("mq_open failed");
		}
	}

	if (mq_send (mymq, argv[1], len, prio) == -1)
		handle_error("mq_send failed");

	mq_close(mymq);
	exit (0);
}

