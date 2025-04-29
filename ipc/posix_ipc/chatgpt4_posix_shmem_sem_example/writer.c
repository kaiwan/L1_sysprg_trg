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
	if (ftruncate(shm_fd, SHM_SIZE) < 0) {
		perror("ftruncate() failed");
		close(shm_fd);
		exit(1);
	}
	void *ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

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

	sem_post(sem);		// Notify reader
	sem_close(sem);
	munmap(ptr, SHM_SIZE);
	close(shm_fd);
	return 0;
}
