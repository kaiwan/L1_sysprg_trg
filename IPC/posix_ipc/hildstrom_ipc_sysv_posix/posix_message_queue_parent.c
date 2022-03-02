/*
 * This demonstrates POSIX message queue IPC for separate processes using a
 * name reference to a message queue. Note that the child execve's a totally
 * separate process.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mqueue.h>

int main(int argc, char **argv)
{
    int rc = 1;
    pid_t cpid = 0;
    mqd_t msgid = 0;
    struct mq_attr attr;
    char *msg = NULL;
    int rv = 0;
    int exec = 0;
    msg = (char*)malloc(1024);
    if (!msg) {
        perror("msg");
        goto cleanup;
    }
    attr.mq_flags = O_RDWR;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 1024;
    attr.mq_curmsgs = 0;
    msgid = mq_open("/msgname", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, &attr);
    if (msgid < 0) {
        perror("mq_open");
        goto cleanup;
    }
    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        goto cleanup;
    }
    else if (cpid == 0) {
        printf("child pid is %d\n", getpid());
        exec = execl("posix_message_queue_child", "posix_message_queue_child", (char *)0);
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
        rv = snprintf(msg, 1024, "1234");
        if (rv <= 0) {
            perror("snprintf");
            goto cleanup;
        }
        rv = mq_send(msgid, msg, strlen(msg), 0);
        if (rv < 0) {
            perror("mq_send");
            goto cleanup;
        }
        wait(0);
    }
    rc = 0;
cleanup:
    if (msgid > 0) {
        mq_close(msgid);
        mq_unlink("/msgname");
    }
    return rc;
}

