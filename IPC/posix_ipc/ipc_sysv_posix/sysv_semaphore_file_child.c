/*
 * This demonstrates semaphore IPC for separate processes using a file
 * system path reference to a semaphore. Note that the semget key is
 * derived from a path and this is separate from the initial forked child.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int main(int argc, char **argv)
{
    int rc = 1;
    key_t key = 0;
    int semid = 0;
    struct sembuf sops[2];
    int rv = 0;
    key = ftok("semfile", 'R');
    if (key < 0) {
        perror("ftok");
        goto cleanup;
    }
    semid = semget(key, 1, 0666);
    if (semid < 0) {
        perror("semget");
        goto cleanup;
    }
    printf("execve'd child pid is %d\n", getpid());
    printf("execve'd child waiting for semaphore zero value\n");
    sops[0].sem_num = 0;
    sops[0].sem_op = 0;
    sops[0].sem_flg = 0;
    rv = semop(semid, sops, 1);
    if (rv < 0) {
        perror("semop");
        goto cleanup;
    }
    printf("execve'd child detected semaphore zero value\n");
    rc = 0;
cleanup:
    return rc;
}

