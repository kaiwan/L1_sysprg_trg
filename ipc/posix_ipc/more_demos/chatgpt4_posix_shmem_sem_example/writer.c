/* The writer must run first */
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "common.h"

int main()
{
	int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if (shm_fd < 0) {
		perror("shm_open() failed");
		exit(1);
	}
	if (ftruncate(shm_fd, SHM_SIZE) < 0) {
		perror("ftruncate() failed");
		close(shm_fd);
		exit(1);
	}
	void *ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	
	// The value 0 here (4th param) implies that the caller will have the semaphore 'locked'
	sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);
	if (sem == SEM_FAILED) {
		if (errno == EEXIST) {
			sem = sem_open(SEM_NAME, 0);
			if (sem == SEM_FAILED) {
				perror("sem_open() failed");
				exit(1);
			}
		}
	}

	const char *message = "Hello from writer process!";
	sprintf(ptr, "%s", message);

	printf("Writer: Message written to shared memory.\n");

	sem_post(sem);		// Unlock; notify reader, blocking reader now proceeds...
	sem_close(sem);
	munmap(ptr, SHM_SIZE);
	close(shm_fd);
	return 0;
}
