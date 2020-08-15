/*
 * This demonstrates message queue IPC for separate processes using a file
 * system path reference to a message queue. Note that the msgget key is
 * derived from a path and the child execve's a totally separate process.
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
    pid_t cpid = 0;
    key_t key = 0;
    int msgid = 0;
    struct msqid_ds msgds;
    struct msgbuf msg;
    int rv = 0;
    int exec = 0;
    key = ftok("msgfile", 'R');
    if (key < 0) {
        perror("ftok");
        goto cleanup;
    }
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid < 0) {
        perror("msgget");
        goto cleanup;
    }
    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        goto cleanup;
    }
    else if (cpid == 0) {
        printf("child pid is %d\n", getpid());
        exec = execve("sysv_message_queue_file_child", NULL, NULL);
        if (exec < 0) {
            perror("execve");
            goto cleanup;
        }
    }
    else {
        printf("parent pid is %d and child pid is %d\n", getpid(), cpid);
        sleep(1);
        printf("parent delay\n");
        sleep(2);
        printf("parent about to send message\n");
        msg.mtype = 1234;
        rv = msgsnd(msgid, &msg, 0, 0);
        if (rv < 0) {
            perror("msgsnd");
            goto cleanup;
        }
        wait(0);
    }
    rc = 0;
cleanup:
    if (msgid > 0)
        msgctl(msgid, IPC_RMID, &msgds);
    return rc;
}

