/*
 * client_sem_as_counting.c
 * Here we demo using the POSIX semaphore as a counting semaphore.
 * Client/server architecture
 *
 * Server:
 * Initilializes and waits for the client to 'connect'; by sending a message into
 * a POSIX 'client request' MQ (or 'mailbox')...
 *
 * Client: [this program]
 * Initialized and sends a message into the 'client request' MQ (or 'mailbox')..
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

static void sem_setup(void)
{
	/* open the semaphore */
	sem = sem_open(SEM_NAME, 0);
	if (sem == SEM_FAILED) {
		perror("sem_open() failed");
		exit(1);
	}
}

/* Send a request to the server by writing a msg into the 'client request' MQ */
static void client_send_request(char *msg, int prio)
{
/*	mqd_t mymq = mq_open(MQNAME, O_RDWR|O_CREAT|O_EXCL, 0644, NULL);// 'attr' as NULL => use defaults
	if (mymq == -1) {
		if (EEXIST == errno) {	// failed as the MQ already exists!
		*/
	mqd_t client_request_mq = mq_open(MQ_NAME, O_WRONLY);
	if (client_request_mq == -1)
		handle_error("mq_open failed");

	/*
	 int mq_send(mqd_t mqdes, const char msg_ptr[.msg_len],
                     size_t msg_len, unsigned int msg_prio);
	  If MQ full, it blocks by default, unless O_NONBLOCK enabled (in the mq_open() oflag param)
	 */
	if (mq_send(client_request_mq, msg, strlen(msg), prio) == -1) {
		mq_close(client_request_mq);
		handle_error("client: mq_send() failed");
	}
	printf("client: msg sent to MQ\n");
	mq_close(client_request_mq);
}

int main(int argc, char **argv)
{
	int prio = 100; /* arbit */

	if (argc != 2) {
		fprintf(stderr, "Usage: %s message-to-send-to-client-request-MQ(ASCII text)\n", argv[0]);
		exit(1);
	}
	// start afresh
	sem_setup();

	/* 'UNLOCK" the mutex.
	 * The counting sem's initial value is higher (SEM_INITVAL) - we programmed this via the
	 * 4th param to the sem_open(). So let the server go
	 * ahead and do the work; once done, 'unlock' the sem (mutex) by
	 * incrementing it (via the sem_post()) so that the client - whose
	 * waiting for it - can get it; UPTO the point where it can't...
	 */
	printf("\nclient: waiting on the semaphore ...\n");
	if (sem_wait(sem) == -1)
		perror("sem_wait() failed");
	// possibly blocking... try to decrement sem; will wait until sem value > 0 ...
	printf("client: got the sem! sending msg to MQ\n");
	client_send_request(argv[1], prio);
	sem_close(sem);

	exit(0);
}
