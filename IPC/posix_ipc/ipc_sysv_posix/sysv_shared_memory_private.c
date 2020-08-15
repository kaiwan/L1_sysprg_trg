/*
 * This demonstrates shared memory IPC and child inheritance of
 * attached shared memory segments. Note that the IPC_PRIVATE shmget key is
 * only useful for parent and child forked processes. Different processes
 * must use an explicit key number, usually provided by ftok().
 *
 * This page is a useful supplement to the man pages.
 * http://www.cs.cf.ac.uk/Dave/C/node27.html
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char **argv)
{
    int rc = 1;
    pid_t cpid = 0;
    int shmid = 0;
    struct shmid_ds shmbuf;
    char *buffer = NULL;
    size_t buffer_size = 4096;
    int size = 0;
    shmid = shmget(IPC_PRIVATE, buffer_size, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        goto cleanup;
    }
    buffer = (char*)shmat(shmid, NULL, 0);
    if (buffer < 0) {
        perror("shmat");
        goto cleanup;
    }
    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        goto cleanup;
    }
    else if (cpid == 0) {
        printf("child pid is %d\n", getpid());
        size = snprintf(buffer, buffer_size, "hello from child");
        if (size <= 0) {
            perror("snprintf");
            goto cleanup;
        }
        printf("child wrote to shared memory buffer: %s\n", buffer);
    }
    else {
        printf("parent pid is %d and child pid is %d\n", getpid(), cpid);
        wait(0);
        printf("parent read from shared memory buffer: %s\n", buffer);
    }
    rc = 0;
cleanup:
    if (buffer > 0)
        shmdt(buffer);
    if (shmid > 0)
        shmctl(shmid, IPC_RMID, &shmbuf);
    return rc;
}

