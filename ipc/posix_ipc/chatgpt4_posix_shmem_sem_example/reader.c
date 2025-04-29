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
	void *ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (!ptr) {
		perror("mmap() failed");
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

	if (sem_wait(sem) < 0) {	// Wait for writer
		perror("sem_wait() failed");
		return -errno;
	}

	printf("Reader: Read message: %s\n", (char *)ptr);

	sem_close(sem);
	sem_unlink(SEM_NAME);	// Clean up
	munmap(ptr, SHM_SIZE);
	close(shm_fd);
	shm_unlink(SHM_NAME);	// Clean up
	return 0;
}
