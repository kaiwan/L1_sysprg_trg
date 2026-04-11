/*
 * appdemo_shmsem.c
 * The 'classic' case: avoid data races on shmem regions by using a semaphore
 * as a mutex.
 * Using POSIX shared memory + POSIX named semaphore for synchronization (as a mutex).
 *

Demo runs:

1. Correct way:
|appdemo_shm_sem_as_mtx $ ./appdemo_shm_sem_as_mtx 1
(Mutex/binary) Semaphore setup and init to value 0
1607336: writing char p for chunk 0 at offset 0 now
Child is waiting to acquire the mutex (sem) now...
1607336: writing char p for chunk 1 at offset 4096 now
1607336: writing char p for chunk 2 at offset 8192 now
1607336: writing char p for chunk 3 at offset 12288 now
1607336: writing char p for chunk 4 at offset 16384 now
1607336: writing char p for chunk 5 at offset 20480 now
1607337: writing char c for chunk 0 at offset 2560 now
1607337: writing char c for chunk 1 at offset 6656 now
1607337: writing char c for chunk 2 at offset 10752 now
1607337: writing char c for chunk 3 at offset 14848 now
1607337: writing char c for chunk 4 at offset 18944 now
1607337: writing char c for chunk 5 at offset 23040 now
|appdemo_shm_sem_as_mtx $ hexdump --canonical /dev/shm/myshm_mtx_demo 
00000000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
00000a00  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
*
00006e00  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
0xa00 = 2560

2. WRONG way:

|appdemo_shm_sem_as_mtx $ ./appdemo_shm_sem_as_mtx 0
(Mutex/binary) Semaphore setup and init to value 0
1607342: writing char p for chunk 0 at offset 0 now
Child is NOT using the mutex (sem) now...
1607343: writing char c for chunk 0 at offset 2560 now
1607342: writing char p for chunk 1 at offset 4096 now
1607343: writing char c for chunk 1 at offset 6656 now
1607342: writing char p for chunk 2 at offset 8192 now
1607343: writing char c for chunk 2 at offset 10752 now
1607342: writing char p for chunk 3 at offset 12288 now
1607343: writing char c for chunk 3 at offset 14848 now
1607342: writing char p for chunk 4 at offset 16384 now
1607343: writing char c for chunk 4 at offset 18944 now
1607342: writing char p for chunk 5 at offset 20480 now
1607343: writing char c for chunk 5 at offset 23040 now
|appdemo_shm_sem_as_mtx $ 
|appdemo_shm_sem_as_mtx $ hexdump --canonical /dev/shm/myshm_mtx_demo 
00000000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
00000a00  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
*
00001000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
00001a00  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
*
00002000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
00002a00  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
*
00003000  70 70 70 70 70 70 70 70  70 70 70 70 70 70 70 70  |pppppppppppppppp|
*
00003a00  63 63 63 63 63 63 63 63  63 63 63 63 63 63 63 63  |cccccccccccccccc|
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
#define LEN 2560

#define SEM_NAME "/appdemo_mtx"	// under /dev/shm
static sem_t *sem;

static inline void sem_setup(void)
{
	sem_unlink(SEM_NAME);  // clean up stale semaphore, ignore errors
	/* Create and open the semaphore */
	sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 0);
	if (sem == SEM_FAILED) {
		perror("sem_open() failed");
		exit(1);
	}
	printf("(Mutex/binary) Semaphore setup and init to value 0\n");
}

/* Write len bytes of char 'letter' to region p_udata starting at offset 'offset'
 * Splits the write into page-sized chunks
 */
static void update_udata(void *p_udata, char letter, off_t offset, size_t len)
{
	const size_t chunk_sz = 4096;	/* one page per iteration */
	size_t num_iters = len / chunk_sz;
	size_t remainder = len % chunk_sz;

	/* Note: from cppcheck:
	portability: 'p_udata' is of type 'void *'. When using void pointers in calculations,
	the behaviour is undefined. Arithmetic operations on 'void *' is a GNU C extension,
	which defines the 'sizeof(void)' to be 0. [arithOperationsOnVoidPointer]
	 */
	for (size_t i = 0; i < num_iters; i++) {
		printf("%d: writing char %c for chunk %zu at offset %zu now\n",
		       getpid(), letter, i, offset + (i * chunk_sz));
		memset(p_udata + offset + (i * chunk_sz), letter, chunk_sz);
		usleep(1000);	/* 1ms */
	}
	if (remainder)
		memset(p_udata + offset + (num_iters * chunk_sz), letter, remainder);
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
			printf("Child is waiting to acquire the mutex (sem) now...\n");
			if (sem_wait(sem) == -1)
				perror("sem_wait() failed");
		}
		else
			printf("Child is NOT using the mutex (sem) now...\n");
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

	// Clean up
	munmap(p_udata, MEMSIZE);
	close(shmfd);
	if (use_mtx)
		sem_close(sem);
	sem_unlink(SEM_NAME);
}
