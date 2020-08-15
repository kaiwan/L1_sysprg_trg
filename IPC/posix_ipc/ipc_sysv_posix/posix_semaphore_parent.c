/*
 * This demonstrates POSIX semaphore IPC for separate processes using a
 * name reference to a semaphore. Note that the child execve's a totally
 * separate process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    int rc = 1;
    pid_t cpid = 0;
    sem_t *sem = NULL;
    int rv = 0;
    int exec = 0;

	// 'named' semaphore; on Linux, it's under a tmpfs /dev/shm/sem.<somename>
    sem = sem_open("/semname", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        goto cleanup;
    }
	system("ls -l /dev/shm/sem*");

    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        goto cleanup;
    }
    else if (cpid == 0) {
        printf("child pid is %d\n", getpid());
        exec = execlp("./posix_semaphore_child", "posix_semaphore_child", NULL);
        if (exec < 0) {
            perror("execlp");
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

