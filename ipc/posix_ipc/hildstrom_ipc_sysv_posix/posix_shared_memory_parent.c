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
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int rc = 1;
	pid_t cpid = 0;
	int shmfd = 0;
	char *buffer = NULL;
	size_t buffer_size = 4096;
	int exec = 0;
	int rv = 0;

	// POSIX shmem pseudofile's created under /dev/shm
	shmfd =
	    shm_open("/shmname", O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
	if (shmfd < 0) {
		perror("shm_open");
		goto cleanup;
	}

	// Set the size of the shmem segment
	rv = ftruncate(shmfd, buffer_size);
	if (rv < 0) {
		perror("ftruncate");
		goto cleanup;
	}

	// void *mmap(void *addr, size_t length, int prot, int flags,
	//                 int fd, off_t offset);
	buffer = mmap(NULL, buffer_size, PROT_READ, MAP_SHARED, shmfd, 0);
	if (buffer < 0) {
		perror("mmap");
		goto cleanup;
	}

	cpid = fork();
	if (cpid < 0) {
		perror("fork");
		goto cleanup;
	} else if (cpid == 0) {
		printf("child pid is %d\n", getpid());
		exec =
		    execl("posix_shared_memory_child",
			  "posix_shared_memory_child", (char *)NULL);
		if (exec < 0) {
			perror("execve");
			goto cleanup;
		}
	} else {
		printf("parent pid is %d and child pid is %d\n", getpid(),
		       cpid);
		wait(0);
		printf("Parent: child has written to the shmfile under /dev/shm/shmname\n"
				"(delay for a few secs... check it out)\n");
		sleep(5);
		printf("parent read from shared memory buffer: %s\n", buffer);
	}
	rc = 0;

 cleanup:
	if (buffer > 0)
		munmap(buffer, buffer_size);
	if (shmfd > 0) {
		close(shmfd);
		shm_unlink("/shmname");
	}
	return rc;
}
