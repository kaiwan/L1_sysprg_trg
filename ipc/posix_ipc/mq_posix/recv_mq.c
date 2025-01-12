/*
 * recv_mq.c

 Simple POSIX Message Queue Producer/Consumer demo.
 This program is the "consumer" (or receiver) app.

 If not already done, mount the 'mqueue' filesystem as root:
 # mkdir /dev/mqueue
 # mount -t mqueue none /dev/mqueue
 $ mount |grep mqueue
 none on /dev/mqueue type mqueue (rw)
 $ 
 
 Can then see that messages have been written by the sender...
 $ ls -l /dev/mqueue/
 total 0
 -rw-r--r-- 1 kaiwan kaiwan 80 May 28 12:15 myposixmq
 $ cat /dev/mqueue/myposixmq 
 QSIZE:30         NOTIFY:0     SIGNO:0     NOTIFY_PID:0
 $ 

 Here, in this simple demo, as there's only one MQ and the send/recv calls are
 synchrounous (blocking by default), there's no chance of a race... If, though,
 we had multiple MQs and multiple concurrent sender/receiver processes (with
 possibly non-blocking IO), then we should employ a POSIX semaphore to handle
 concurrency.

 (c) kaiwan billimoria, MIT.
 */
#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define fatal_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define MQNAME "/myposixmq"

int main(int argc, char **argv)
{
	mqd_t mymq;
	struct mq_attr attr;

	// Create a POSIX MQ here: /dev/mqueue/MQNAME
	mymq = mq_open(MQNAME, O_RDWR|O_CREAT|O_EXCL, 0644, NULL);	// 'attr' as NULL => use defaults
	if (mymq == -1) {
		if (EEXIST == errno) {	// failed as the MQ already exists!
			mymq = mq_open(MQNAME, O_RDONLY);
			if (mymq == -1)
				fatal_error("mq_open failed");
		}
	}

	/* Determine max msg size (typically 8192 bytes); allocate buffer to receive msg */
	if (mq_getattr(mymq, &attr) == -1) {
		mq_close(mymq);
		fatal_error("mq_getattr failed");
	}
	char *buf = calloc(attr.mq_msgsize, 1);
	if (!buf) {
		mq_close(mymq);
		fatal_error("calloc failed");
	}

	while (1) {
		ssize_t nr = 0;
		unsigned prio = 0;

		/* ssize_t mq_receive(mqd_t mqdes, char msg_ptr[.msg_len],
                          size_t msg_len, unsigned int *msg_prio); 
		   3rd param len must be attr.mq_msgsize (usually 8192)
		   If MQ empty, it blocks by default, unless O_NONBLOCK enabled (in the mq_open() oflag param)
		 */
		if ((nr = mq_receive(mymq, buf, attr.mq_msgsize, &prio)) == -1) {
			free(buf);
			mq_close(mymq);
			fatal_error("mq_receive failed");
		}
		printf("%s: received msg: %60s [%5ld bytes, prio %5u]\n",
			argv[0], buf, nr, prio);
	}

	free(buf);
	mq_close(mymq);
	exit(0);
}
