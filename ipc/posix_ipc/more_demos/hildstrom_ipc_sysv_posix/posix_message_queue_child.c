/*
 * This demonstrates POSIX message queue IPC for separate processes using a
 * name reference to a message queue. Note that this is separate from the
 * initial forked child.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

static const char mqname[] = "/posix_mq";
int main(int argc, char **argv)
{
	int rc = 1;
	mqd_t msgid = 0;
	char *msg = NULL;
	ssize_t rv = 0;

	msg = (char *)malloc(1024);
	if (!msg) {
		perror("malloc");
		goto cleanup;
	}

	// We expect the POSIX MQ to be existing...
	msgid = mq_open(mqname, O_RDONLY);
	if (msgid < 0) {
		perror("mq_open");
		goto cleanup;
	}

	printf("execve'd child pid is %d; sleep for 10s...\n", getpid());
	sleep(10);
	printf("execve'd child waiting for message\n");
	rv = mq_receive(msgid, msg, 1024, NULL);
	if (rv < 0) {
		perror("mq_receive");
		goto cleanup;
	}
	printf("execve'd child received message %s\n", msg);
	rc = 0;

cleanup:
	free(msg);
	if (msgid > 0)
		mq_close(msgid);
	return rc;
}
