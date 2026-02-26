/* The writer must run first */
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"

int main()
{
	int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
	if (shm_fd < 0) {
		perror("shm_open() failed");
		exit(1);
	}
	void *ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (!ptr) {
		perror("mmap() failed");
		close(shm_fd);
		exit(1);
	}

	sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 0);
	if (sem == SEM_FAILED) {
		if (errno == EEXIST) {
			sem = sem_open(SEM_NAME, 0);
			if (sem == SEM_FAILED) {
				perror("sem_open() failed");
				exit(1);
			}
		}
	}

	if (sem_wait(sem) < 0) {	// Writer has the lock initially, wait for writer
		perror("sem_wait() failed");
		return -errno;
	}
	// Now we have the lock, read the message from shared memory
	// Read the message from shared memory and print it
	printf("Reader: Read message: %s\n", (char *)ptr);

	sem_close(sem);
	munmap(ptr, SHM_SIZE);
	close(shm_fd);

#define	REMOVE_OBJECTS 0
#if REMOVE_OBJECTS == 1
	sem_unlink(SEM_NAME);	// Clean up
	shm_unlink(SHM_NAME);
#endif
	return 0;
}
