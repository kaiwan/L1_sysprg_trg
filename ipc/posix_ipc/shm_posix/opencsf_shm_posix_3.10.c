/* Code Listing 3.10:
   Using POSIX shared memory to exchange data between processes
   https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/ShMem.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>		/* For O_* constants */
#include <sys/mman.h>		// mmap()
#include <sys/stat.h>		/* For mode constants */
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHM_NAME "/opencsf_shm"

struct permission {
	int user;
	int group;
	int other;
};

int main(void)
{
/* Create unsized shared memory object;
   return value is a file descriptor */
	int shmfd =
	    shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if (shmfd == -1) {
		if (errno == EEXIST) {
			shmfd = shm_open(SHM_NAME, 0, 0644);
			if (shmfd == -1) {
				fprintf(stderr, "shm_open() failed\n");
				exit(1);
			}
		}
	}

	/* Resize the region to store 1 struct instance */
	if (ftruncate(shmfd, sizeof(struct permission)) == -1) {
		perror("ftruncate failed");
		shm_unlink(SHM_NAME);
		exit(1);
	}
	//assert(ftruncate(shmfd, sizeof(struct permission)) != -1);

/* Map the object into memory so file operations aren't needed */
	struct permission *perm = mmap(NULL, sizeof(struct permission),
				       PROT_READ | PROT_WRITE, MAP_SHARED,
				       shmfd, 0);
	assert(perm != MAP_FAILED);

/* Create a child process and write to the mapped/shared region */
	pid_t child_pid = fork();
	if (child_pid == 0) {
		perm->user = 6;
		perm->group = 4;
		perm->other = 0;

		/* Unmap and close the child's shared memory access */
		munmap(perm, sizeof(struct permission));
		close(shmfd);
		return 0;
	}

	/* Make the parent wait until the child has exited */
	wait(NULL);

/* Read from the mapped/shared memory region */
	printf("Permission bit-mask: 0%d%d%d\n", perm->user, perm->group,
	       perm->other);

/* Unmap, close, and delete the shared memory object */
	munmap(perm, sizeof(struct permission));
	close(shmfd);
	shm_unlink(SHM_NAME);
}
