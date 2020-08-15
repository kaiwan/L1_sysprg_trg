/*
 * This demonstrates shared memory IPC for separate processes using a file
 * system path reference to shared memory. Note that the shmget key is
 * derived from a path and this is separate from the initial forked child.
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
    key_t key = 0;
    int shmid = 0;
    char *buffer = NULL;
    size_t buffer_size = 4096;
    int size = 0;
    key = ftok("shmfile", 'R');
    if (key < 0) {
        perror("ftok");
        goto cleanup;
    }
    shmid = shmget(key, buffer_size, 0666);
    if (shmid < 0) {
        perror("shmget");
        goto cleanup;
    }
    buffer = (char*)shmat(shmid, NULL, 0);
    if (buffer < 0) {
        perror("shmat");
        goto cleanup;
    }
    printf("execve'd child pid is %d\n", getpid());
    size = snprintf(buffer, buffer_size, "hello from execve'd child");
    if (size <= 0) {
        perror("snprintf");
        goto cleanup;
    }
    printf("execve'd child wrote to shared memory buffer: %s\n", buffer);
    rc = 0;
cleanup:
    if (buffer > 0)
        shmdt(buffer);
    return rc;
}

