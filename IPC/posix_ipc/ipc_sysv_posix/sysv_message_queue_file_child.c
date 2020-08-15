/*
 * This demonstrates message queue IPC for separate processes using a file
 * system path reference to a message queue. Note that the msgget key is
 * derived from a path and this is separate from the initial forked child.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msgbuf {
    long mtype;
    char mtext[1];
};

int main(int argc, char **argv)
{
    int rc = 1;
    key_t key = 0;
    int msgid = 0;
    struct msgbuf msg;
    ssize_t rv = 0;
    key = ftok("msgfile", 'R');
    if (key < 0) {
        perror("ftok");
        goto cleanup;
    }
    msgid = msgget(key, 0666);
    if (msgid < 0) {
        perror("msgget");
        goto cleanup;
    }
    printf("execve'd child pid is %d\n", getpid());
    printf("execve'd child waiting for message\n");
    rv = msgrcv(msgid, &msg, 0, 0, 0);
    if (rv < 0) {
        perror("msgrcv");
        goto cleanup;
    }
    printf("execve'd child received message type %d\n", msg.mtype);
    rc = 0;
cleanup:
    return rc;
}

