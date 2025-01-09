/*
 * appdemo_shmsem.c
 * The 'classic' case: avoid data races on shmem regions by using a semaphore
 * as a mutex.
 * Using POSIX shared memory + POSIX named semaphore for synchronization (as a mutex).
 *

Demo runs:

1. Correct way:
$ ./appdemo_shmsem 1
56930: writing char p for 10485760 bytes at offset 0 now
56931: writing char c for 10485760 bytes at offset 1048576 now
appdemo_shmsem $ hexdump --canonical /dev/shm/myshm 
00000000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
00100000  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
*
00b00000  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
01400000
$ 

2. WRONG way (no sem):
$ ./appdemo_shmsem 0
56988: writing char p for 10485760 bytes at offset 0 now
56989: writing char c for 10485760 bytes at offset 1048576 now
appdemo_shmsem $ hexdump --canonical /dev/shm/myshm 
00000000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
0036f000  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
*
003d3000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
00434000  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
*
00457000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
00475000  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
*
[ ... ]

 * Kaiwan NB
 * License: MIT
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
#include <string.h>

#define SHM_NAME "/myshm_mtx_demo"
#define SHM_MODE 0640
#define MEMSIZE 1024*1024*20
#define LEN 1024*1024

#define SEM_NAME "/appdemo_mtx"	// under /dev/shm
static sem_t *sem;

static void sem_setup(void)
{
	/* Create and open the semaphore */
	sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 0);
	if (sem == SEM_FAILED) {
		if (errno == EEXIST) {
			sem = sem_open(SEM_NAME, 0);
			if (sem == SEM_FAILED) {
				perror("sem_open() failed");
				exit(1);
			}
		}
	}
	printf("(Mutex) Semaphore setup and init to value 0\n");
}

/* Write len bytes of char 'letter' to region p_udata starting at offset 'offset' */
static void update_udata(void *p_udata, char letter, off_t offset, size_t len)
{
	printf("%d: writing char %c for %zu bytes at offset %zu now\n",
	       getpid(), letter, len, offset);
	memset(p_udata + offset, letter, len);
	/* Note: from cppcheck:
	portability: 'p_udata' is of type 'void *'. When using void pointers in calculations,
	the behaviour is undefined. Arithmetic operations on 'void *' is a GNU C extension,
	which defines the 'sizeof(void)' to be 1. [arithOperationsOnVoidPointer]
	 */	   
}

int main(int argc, char **argv)
{
	int use_mtx = 1;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s 1|0\n\
 1 => use the semaphore (as a mutex lock); correct, expected behavior\n\
 0 => DON'T use the semaphore (as a mutex lock); incorrect, buggy, data races ensue!\n", argv[0]);
		exit(1);
	}
	if (atoi(argv[1]) != 1)
		use_mtx = 0;

	// start afresh
	shm_unlink(SHM_NAME);

	/* Create unsized shared memory object;
	 * return value is a file descriptor */
	int shmfd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, SHM_MODE);
	if (shmfd == -1) {
		if (errno == EEXIST) {
			shmfd = shm_open(SHM_NAME, 0, SHM_MODE);
			if (shmfd == -1) {
				fprintf(stderr, "shm_open() failed\n");
				exit(1);
			}
		}
	}

	/* Resize the region to MEMSIZE bytes */
	if (ftruncate(shmfd, MEMSIZE) == -1) {
		perror("ftruncate failed");
		shm_unlink(SHM_NAME);
		exit(1);
	}

	/* Map the object into memory so file operations aren't needed 
	   void *mmap(void addr[.length], size_t length, int prot, int flags,
	   int fd, off_t offset);
	 */
	void *p_udata = mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
			     shmfd, 0);
	assert(p_udata != MAP_FAILED);

	sem_setup();

/* Create a child process and write to the mapped/shared region */
	pid_t child_pid = fork();
	switch (child_pid) {
	case -1:
		perror("fork failed");
		exit(1);
	case 0:		// Child
		if (use_mtx) {
			/* 
			 * (Attempt to) 'LOCK" the mutex.
			 * Attempt to 'lock' the (binary) sem/mutex by attempting to
			 * decrement it via the sem_wait(); if it's value is 0 
			 * - the default - it's forced to wait on it being
			 * incremeneted to 1 by the parent calling sem_post()
			 */
			if (sem_wait(sem) == -1)
				perror("sem_wait() failed");
		}
		//static void update_udata(void *p_udata, char letter, off_t offset, size_t len)
		update_udata(p_udata, 'c', LEN, LEN * 10);
		munmap(p_udata, MEMSIZE);
		close(shmfd);
		if (use_mtx)
			sem_close(sem);
		exit(0);
	default:	// Parent
		/* The sem init value is 0; go ahead, do the work, then increment it
		 * so that the child - whose waiting for it - can get it
		 */
		//static void update_udata(void *p_udata, char letter, off_t offset, size_t len)
		update_udata(p_udata, 'p', 0, LEN * 10);
		if (use_mtx) {
			/* 'UNLOCK" the mutex.
			 * The sem (mutex's) initial value is 0 - we programmed this via the
			 * 4th param to the sem_open(). So let the parent go ahead and do the
			 * work; once done, 'unlock' the sem (mutex) by incrementing it (via
			 * the sem_post()) so that the child - whose waiting for it - can get it.
			 */
			if (sem_post(sem) == -1)
				perror("sem_post() failed");
		}
		wait(0);
	}

	/* Unmap, close, and delete the shared memory object */
	munmap(p_udata, MEMSIZE);
	close(shmfd);
	if (use_mtx)
		sem_close(sem);
}
