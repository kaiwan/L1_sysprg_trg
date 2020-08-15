/*
 * This demonstrates shared memory IPC for separate processes using a file
 * system path reference to shared memory. Note that the shmget key is
 * derived from a path and the child execve's a totally separate process.
 *
 * This page is a useful supplement to the man pages.
 * http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/shm/example-2.html
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
    key_t key = 0;
    int shmid = 0;
    struct shmid_ds shmbuf;
    char *buffer = NULL;
    size_t buffer_size = 4096;
    int exec = 0;
    key = ftok("shmfile", 'R');
    if (key < 0) {
        perror("ftok");
        goto cleanup;
    }
    shmid = shmget(key, buffer_size, IPC_CREAT | 0666);
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
        exec = execve("sysv_shared_memory_file_child", NULL, NULL);
        if (exec < 0) {
            perror("execve");
            goto cleanup;
        }
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

