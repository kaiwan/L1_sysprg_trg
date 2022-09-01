/*
 * sender_mq.c

 Simple POSIX Message Queue Producer/Consumer demo.
 This program is the "producer" (or sender) app.

 Should mount the 'mqueue' filesystem as root:
 # mkdir /dev/mq
 # mount -t mqueue none /dev/mqueue

 Can then see that messages have been written...
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

(c) 2012, kaiwan billimoria.
License: MIT

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

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define MQNAME "/myposixmq"

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
	printf("prio: %d, max prio: %ld\n", prio, sysconf(_SC_MQ_PRIO_MAX));

	mymq = mq_open(MQNAME, O_RDWR | O_CREAT | O_EXCL, 0644, NULL);	// 'attr' as NULL => use defaults
	if (mymq == -1) {
		if (EEXIST == errno) {	// failed as the MQ already exists!
			mymq = mq_open(MQNAME, O_RDWR);
			if (mymq == -1)
				handle_error("mq_open failed");
		}
	}

	if (mq_send(mymq, argv[1], len, prio) == -1) {
		mq_close(mymq);
		handle_error("mq_send failed");
	}

	mq_close(mymq);
	exit(0);
}
