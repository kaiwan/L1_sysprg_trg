/*
 * server_sem_as_counting.c
 * Here we demo using the POSIX semaphore as a counting semaphore.
 * Client/server architecture
 *
 * Server: [this program]
 * Initilializes and waits for the client to 'connect'; by sending a message into
 * a POSIX 'client request' MQ (or 'mailbox')...
 *
 * Client:
 * Initilaized and sends a message into the 'client request' MQ (or 'mailbox')..
 * This of course unblocks the server which then performs the 'work' - we just sleep
 * for 3s - and then the server 'unlocks' by incrementing the sem.
 *
 * The key point: the server maintains a semaphore 'count' - a value it initializes
 * the sem to... (this is passed as a param). Once this many clients 'connect', the
 * next one *has to wait* until existing clients finish... this behaviour is implemented
 * via the counting semaphore!
 *
 * Kaiwan NB
 * License: MIT
 */
#include "sem_counting_common.h"
static sem_t *sem;

static void sem_setup(int max_sem_count)
{
	/* Create and open the semaphore */
//#define SEM_INITVAL	3
	printf("\nserver: initializing sem value to %d; this, in effect, is the \
max 'queue length' or 'count' allowed by this (counting) semaphore ...\n", max_sem_count);

	sem = sem_open(SEM_NAME, O_CREAT|O_EXCL, 0600, max_sem_count);
	if (sem == SEM_FAILED) {
		if (errno == EEXIST) {
			sem = sem_open(SEM_NAME, 0);
			if (sem == SEM_FAILED) {
				fprintf(stderr, "sem_open() failed\n");
				exit(1);
			}
		}
	}
	printf("(Counting) Semaphore open..\n");
}

static void receive_client_msg(void)
{
	struct mq_attr attr;

	mqd_t client_request_mq = mq_open(MQ_NAME, O_RDWR|O_CREAT|O_EXCL, 0644, NULL);
		// 'attr' as NULL => use defaults
	if (client_request_mq == -1) {
		if (EEXIST == errno) {	// failed as the MQ already exists!
			client_request_mq = mq_open(MQ_NAME, O_RDWR);
			if (client_request_mq == -1)
				handle_error("mq_open failed");
		}
	}

	/* Determine max msg size (typically 8192 bytes); allocate buffer to receive msg */
	if (mq_getattr(client_request_mq, &attr) == -1)
		handle_error("mq_getattr failed");
	char *buf = calloc(attr.mq_msgsize, 1);
	if (!buf) {
		mq_close(client_request_mq);
		handle_error("calloc failed");
	}

	ssize_t nr = 0;
	unsigned prio = 0;

	/* ssize_t mq_receive(mqd_t mqdes, char msg_ptr[.msg_len],
                         size_t msg_len, unsigned int *msg_prio); 
	   3rd param len must be attr.mq_msgsize (usually 8192)
	   If MQ empty, it blocks by default, unless O_NONBLOCK enabled (in the mq_open() oflag param)
	 */
	printf("server: awaiting msg from client...\n");
	if ((nr = mq_receive(client_request_mq, buf, attr.mq_msgsize, &prio)) == -1) {
		mq_close(client_request_mq);
		handle_error("mq_receive failed");
	}
	printf("server: Received:\n \"%s\" [%ld bytes, prio %u]\n", buf, nr, prio);

	free(buf);
	mq_close(client_request_mq);
}

int main(int argc, char **argv)
{
	int i = 1;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s max-sem-count\n", argv[0]);
		exit(1);
	}

	// start afresh
	mq_unlink(MQ_NAME);
	sem_unlink(SEM_NAME);
	sem_setup(atoi(argv[1]));

	while (1) {
		int semval;

		// read - wait upon - a msg from the 'client request' MQ...
		receive_client_msg();

		/* 'UNLOCK" the mutex.
		 * The counting sem's initial value is higher (argv 1) - we
		 * programmed this via the 4th param to the sem_open(). 
		 * So let the server go ahead and do the 'work'; once done,
		 * 'unlock' the sem (mutex) by incrementing it (via the
		 * sem_post()) so that the client - whose waiting for it - can
		 * get it; UPTO the point where it can't, when the count is higher
		 * than the max allowed...
		 */
		if (sem_getvalue(sem, &semval) == -1)
			perror("sem_getvalue() failed");
		printf("server %d: sem val = %d\n", i ++, semval);

		// Deliberately emulate taking some time, for demo purposes...
		printf(" server: performing 'work' for 3s... ");
		fflush(stdout);
		sleep(3); //(5);
		printf(" done! (will now 'unlock' by incrementing the sem...)\n\n");

		if (sem_post(sem) == -1) // increments sem val
			perror("sem_post() failed");
	}
	sem_close(sem);

	exit(0);
}
