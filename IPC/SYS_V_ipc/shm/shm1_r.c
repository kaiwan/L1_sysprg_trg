/*
* shm1_r.c
*
* Simple shared memory demo app.
* Consumer: this program attaches to the existing shared memory segment (we 
* assume the producer process has run earlier), and reads the user data from it.
* License: MIT
*/
#include "hdr_shm.h"
#include <signal.h>

static void sighdlr(int signum)
{
	printf("-- In sighdlr, sig #%d\n", signum);
}

int main(int argc, char **argv)
{
	int shm_id;		/* shared memory id */
	key_t shm_key;		/* key for shared memory access */
	void *shm_addr;		/* virtual address of shared memory */
	char *next;		/* pointer to next string */
	int n;			/* for loop counter */
	int incr;		/* increment for next string */
	int rv;			/* current function return value */

	struct sigaction act;
	//sigset_t sigset, prevset;

	/* set up signals: trap SIGINT */
	act.sa_handler = sighdlr;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &act, 0) == -1) {
		perror(argv[0]);
		exit(1);
	}

	/* Fetch the key value */
	shm_key = get_key();
	if (shm_key == -1) {
		fprintf(stderr, "%s: ftok() failed, cannot get key value\n",
			argv[0]);
		exit(1);
	}

	/* Define and attach to the shared memory */
	shm_id = shmget(shm_key, 0, 0);
	if (shm_id < 0) {
		perror("shmget failed");
		exit(1);
	}
	printf("%s: shmem segment successfully created / accessed. ID=%d\n",
	       argv[0], shm_id);

	shm_addr = shmat(shm_id, 0, 0);
	if (shm_addr == (void *)-1) {
		perror("shmat failed");
		exit(1);
	}
	printf("%s: Attached successfully to shmem segment at %p\n",
	       argv[0], shm_addr);

	/* Print strings  */
	next = shm_addr;
	for (n = 0; n < NUM; n++) {
		puts(next);
		incr = strlen(next) + 1;
		next += incr;
	}

#if 1
	printf("%s: Pausing ... press ^C to continue... \n", argv[0]);
	pause();
#endif

	/* Detach from and (optionally) delete shared memory segment */
	rv = shmdt(shm_addr);
	if (rv < 0) {
		perror("shmdt failed");
		exit(1);
	}
#if 0
	rv = shmctl(shm_id, IPC_RMID, 0);
	if (rv < 0) {
		printf("%s: cannot delete shared memory\n", argv[0]);
		exit(1);
	}
	printf("%s: deleted shmem segment.\n", argv[0]);
#endif
	exit(0);
}
