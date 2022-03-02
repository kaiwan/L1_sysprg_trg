/*
 * This demonstrates POSIX semaphore IPC for separate processes using a
 * name reference to a semaphore. Note that the child execve's a totally
 * separate process.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
    int rc = 1;
    pid_t cpid = 0;
    sem_t *sem = NULL;
    int rv = 0;
    int exec = 0;
    sem = sem_open("/semname", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        goto cleanup;
    }
    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        goto cleanup;
    }
    else if (cpid == 0) {
        printf("child pid is %d\n", getpid());
        exec = execl("posix_semaphore_child", "posix_semaphore_child", (char *)NULL);
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
        printf("parent about to increment semaphore to non-zero\n");
        rv = sem_post(sem);
        if (rv < 0) {
            perror("sem_post");
            goto cleanup;
        }
        wait(0);
    }
    rc = 0;
cleanup:
    if (sem != SEM_FAILED) {
        sem_close(sem);
        sem_unlink("/semname");
    }
    return rc;
}

