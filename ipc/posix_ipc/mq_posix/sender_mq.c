/*
 * sender_mq.c

 Simple POSIX Message Queue Producer/Consumer demo.
 This program is the "producer" (or sender) app.

 If not already done, mount the 'mqueue' filesystem as root:
 # mkdir /dev/mqueue
 # mount -t mqueue none /dev/mqueue
 $ mount |grep mqueue
 none on /dev/mqueue type mqueue (rw)
 
 $ ./sender_mq
 ...
 Can then see that messages have been written...
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

***
Notice that, in the mq_send() API, the parameter to the message is a ptr to a string:
int mq_send(mqd_t mqdes, const char *msg_ptr,
                     size_t msg_len, unsigned int msg_prio);
So can one send stuff besides ASCII? Yes, via a structure too.
FYI, a sample prg showing how to do so can be found here:
https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/MQueues.html
(Code listing 3.9)
***
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
#define MQNAME "/myposixmq" // actually under /dev/mqueue/myposixmq

int main(int argc, char **argv)
{
	mqd_t mymq;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s \"msg to send\" priority\n",
			argv[0]);
		exit(1);
	}

	int len = strlen(argv[1]);
	if (len <= 0) {
		fprintf(stderr, "%s: invalid message? Aborting...\n", argv[0]);
		exit(1);
	}

	unsigned prio = atoi(argv[2]);
	if ((prio < 0) || (prio > sysconf(_SC_MQ_PRIO_MAX))) {
		fprintf(stderr, "%s: invalid priority value, aborting...\n",
			argv[0]);
		exit(1);
	}
	printf("%s: sent a msg to MQ at prio %5d (max prio=%ld)\n",
		argv[0], prio, sysconf(_SC_MQ_PRIO_MAX));

	mymq = mq_open(MQNAME, O_RDWR|O_CREAT|O_EXCL, 0644, NULL);	// 'attr' as NULL => use defaults
	if (mymq == -1) {
		if (EEXIST == errno) {	// failed as the MQ already exists!
			mymq = mq_open(MQNAME, O_WRONLY);
			if (mymq == -1)
				fatal_error("mq_open failed");
		}
	}

	/*
	 int mq_send(mqd_t mqdes, const char msg_ptr[.msg_len],
                     size_t msg_len, unsigned int msg_prio);
	  If MQ full, it blocks by default, unless O_NONBLOCK enabled (in the mq_open() oflag param)
	 */
	if (mq_send(mymq, argv[1], len, prio) == -1) {
		mq_close(mymq);
		fatal_error("mq_send failed");
	}

	mq_close(mymq);
	exit(0);
}
