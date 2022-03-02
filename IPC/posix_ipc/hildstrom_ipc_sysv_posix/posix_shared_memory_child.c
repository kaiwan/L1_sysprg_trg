/*
 * This demonstrates POSIX shared memory IPC for separate processes using a
 * name reference to shared memory. Note that this is separate from the
 * initial forked child.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int rc = 1;
    int shmfd = 0;
    char *buffer = NULL;
    size_t buffer_size = 4096;
    int size = 0;
    shmfd = shm_open("/shmname", O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (shmfd < 0) {
        perror("shm_open");
        goto cleanup;
    }
    buffer = mmap(NULL, buffer_size, PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (buffer < 0) {
        perror("mmap");
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
        munmap(buffer, buffer_size);
    if (shmfd > 0)
        close(shmfd);
    return rc;
}

