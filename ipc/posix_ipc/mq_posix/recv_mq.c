/*
 * recv_mq.c

 Simple POSIX Message Queue Producer/Consumer demo.
 This program is the "consumer" (or receiver) app.

 Should mount the 'mqueue' filesystem as root:
 # mkdir /dev/mqueue
 # mount -t mqueue none /dev/mqueue

 Can then see that messages have been written by the sender...
 $ mount |grep mqueue
 none on /dev/mqueue type mqueue (rw)
 $ 
 
 $ l /dev/mqueue/
 total 0
 -rw-r--r-- 1 kaiwan kaiwan 80 May 28 12:15 myposixmq
 $ cat /dev/mqueue/myposixmq 
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
	struct mq_attr attr;

	mymq = mq_open(MQNAME, O_RDWR | O_CREAT | O_EXCL, 0644, NULL);	// 'attr' as NULL => use defaults
	if (mymq == -1) {
		if (EEXIST == errno) {	// failed as the MQ already exists!
			mymq = mq_open(MQNAME, O_RDWR);
			if (mymq == -1)
				handle_error("mq_open failed");
		}
	}

	/* Determine max msg size (typically 8192 bytes); allocate buffer to receive msg */
	if (mq_getattr(mymq, &attr) == -1)
		handle_error("mq_getattr failed");
	char *buf = calloc(attr.mq_msgsize, 1);
	if (!buf) {
		mq_close(mymq);
		handle_error("calloc failed");
	}

	while (1) {
		ssize_t nr = 0;
		unsigned prio = 0;

		if ((nr = mq_receive(mymq, buf, attr.mq_msgsize, &prio)) == -1) {
			mq_close(mymq);
			handle_error("mq_receive failed");
		}
		printf("%s: Received:\n \"%s\" [%ld bytes, prio %u]\n", argv[0],
		       buf, nr, prio);
	}

	free(buf);
	mq_close(mymq);
	exit(0);
}
