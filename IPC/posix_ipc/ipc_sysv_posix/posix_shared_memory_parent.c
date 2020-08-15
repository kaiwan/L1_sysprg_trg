/*
 * This demonstrates POSIX shared memory IPC for separate processes using a
 * name reference to shared memory. Note that the child execve's a totally
 * separate process.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    int rc = 1;
    pid_t cpid = 0;
    int shmfd = 0;
    char *buffer = NULL;
    size_t buffer_size = 4096;
    int exec = 0;
    int rv = 0;

	// 'named' shm pbject; on Linux, it's under a tmpfs /dev/shm/<somename>
    shmfd = shm_open("/shmname", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (shmfd < 0) {
        perror("shm_open");
        goto cleanup;
    }
    rv = ftruncate(shmfd, buffer_size);
	system("ls -l /dev/shm/");

    if (rv < 0) {
        perror("ftruncate");
        goto cleanup;
    }
    buffer = mmap(NULL, buffer_size, PROT_READ, MAP_SHARED, shmfd, 0);
    if (buffer < 0) {
        perror("mmap");
        goto cleanup;
    }
	printf("shmem region uva = %p\n", buffer);

    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        goto cleanup;
    }
    else if (cpid == 0) {
        printf("child pid is %d\n", getpid());
        exec = execlp("./posix_shared_memory_child", "posix_shared_memory_child", NULL);
        if (exec < 0) {
            perror("execl failed");
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
        munmap(buffer, buffer_size);
    if (shmfd > 0) {
        close(shmfd);
//        shm_unlink("/shmname");
    }
	while(1)
		pause();
    return rc;
}

