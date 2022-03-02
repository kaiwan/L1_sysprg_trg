/*
 * This demonstrates POSIX semaphore IPC for separate processes using a
 * name reference to a semaphore. Note that this is separate from the
 * initial forked child.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
    int rc = 1;
    sem_t *sem = NULL;
    int rv = 0;
    sem = sem_open("/semname", O_RDWR);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        goto cleanup;
    }
    printf("execve'd child pid is %d\n", getpid());
    printf("execve'd child waiting for semaphore non-zero value\n");
    rv = sem_wait(sem);
    if (rv < 0) {
        perror("sem_wait");
        goto cleanup;
    }
    printf("execve'd child detected semaphore non-zero value\n");
    rc = 0;
cleanup:
    if (sem != SEM_FAILED)
        sem_close(sem);
    return rc;
}

