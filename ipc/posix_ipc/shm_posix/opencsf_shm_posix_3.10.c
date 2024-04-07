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

struct udata {
	size_t lat, longt, alt;
	char location_name[128];
	int gps_fix;
};

int main(void)
{
/* Create unsized shared memory object;
   return value is a file descriptor */
	int shmfd =
	    shm_open(SHM_NAME, O_CREAT|O_EXCL|O_RDWR, 0600); //S_IRUSR|S_IWUSR);
	if (shmfd == -1) {
		if (errno == EEXIST) {
			shmfd = shm_open(SHM_NAME, 0, 0600);
			if (shmfd == -1) {
				fprintf(stderr, "shm_open() failed\n");
				exit(1);
			}
		}
	}

	/* Resize the region to store 1 struct instance */
	if (ftruncate(shmfd, sizeof(struct udata)) == -1) {
		perror("ftruncate failed");
		shm_unlink(SHM_NAME);
		exit(1);
	}
	//assert(ftruncate(shmfd, sizeof(struct udata)) != -1);

/* Map the object into memory so file operations aren't needed */
	struct udata *udata = mmap(NULL, sizeof(struct udata),
				       PROT_READ | PROT_WRITE, MAP_SHARED,
				       shmfd, 0);
	assert(udata != MAP_FAILED);
	printf("Location data before mod: (%zu,%zu,%zu)\n",
	udata->lat, udata->longt, udata->alt);

/* Create a child process and write to the mapped/shared region */
	pid_t child_pid = fork();
	if (child_pid == 0) {
		udata->lat = 77;
		udata->longt = 13;
		udata->alt = 900;

		/* Unmap and close the child's shared memory access */
		munmap(udata, sizeof(struct udata));
		close(shmfd);
		exit(0);
	}

	/* Make the parent wait until the child has exited */
	wait(NULL);

/* Read from the mapped/shared memory region */
	printf("Location data after mod: (%zu,%zu,%zu)\n",
	udata->lat, udata->longt, udata->alt);

/* Unmap, close, and delete the shared memory object */
	munmap(udata, sizeof(struct udata));
	close(shmfd);
#if 1
	shm_unlink(SHM_NAME);
#endif
}
