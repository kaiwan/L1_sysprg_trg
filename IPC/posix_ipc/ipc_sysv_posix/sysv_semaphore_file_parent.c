/*
 * This demonstrates semaphore IPC for separate processes using a file
 * system path reference to a semaphore. Note that the semget key is
 * derived from a path and the child execve's a totally separate process.
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
    pid_t cpid = 0;
    key_t key = 0;
    int semid = 0;
    struct sembuf sops[2];
    int rv = 0;
    int exec = 0;
    key = ftok("semfile", 'R');
    if (key < 0) {
        perror("ftok");
        goto cleanup;
    }
    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget");
        goto cleanup;
    }
    sops[0].sem_num = 0;
    sops[0].sem_op = 1;
    sops[0].sem_flg = 0;
    rv = semop(semid, sops, 1);
    if (rv < 0) {
        perror("semop");
        goto cleanup;
    }
    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        goto cleanup;
    }
    else if (cpid == 0) {
        printf("child pid is %d\n", getpid());
        exec = execve("sysv_semaphore_file_child", NULL, NULL);
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
        printf("parent about to decrement semaphore back to zero\n");
        sops[0].sem_op = -1;
        rv = semop(semid, sops, 1);
        if (rv < 0) {
            perror("semop");
            goto cleanup;
        }
        wait(0);
    }
    rc = 0;
cleanup:
    if (semid > 0)
        semctl(semid, 1, IPC_RMID);
    return rc;
}

