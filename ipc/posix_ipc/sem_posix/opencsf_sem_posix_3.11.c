/* Code Listing 3.11:
   Creating and using a named POSIX semaphore to control the timing of parent/child execution
   https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/IPCSems.html#cl3-11
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>		/* For O_* constants */
#include <sys/stat.h>		/* For mode constants */
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SEM_NAME "/opencsf_sem" // under /dev/shm

int main(void)
{
/* Create and open the semaphore; initialize it to 0 - the last param */
	sem_t *sem =
	    sem_open(SEM_NAME, O_CREAT|O_EXCL, 0600, 0);
	if (sem == SEM_FAILED) {
		if (errno == EEXIST) {
			sem = sem_open(SEM_NAME, 0);
			if (sem == SEM_FAILED) {
				fprintf(stderr, "sem_open() failed\n");
				exit(1);
			}
		}
	}
//	assert(sem != SEM_FAILED);

/* Fork to create the child process */
	pid_t child_pid = fork();
	assert(child_pid != -1);

/* Note the child inherits a copy of the semaphore connection */

/* Child process: wait for semaphore, print "second", then exit
 * As the semaphore's init to 0, the child will wait on the decrement - as
 * the rule is that we can't lower the sem value below 0
 */
	if (child_pid == 0) {
		sem_wait(sem); // wait for "lock"
		printf("second\n");
		sem_close(sem);
		exit(0);
	}

	/* Parent prints then posts to the semaphore and waits on child */
	printf("first\n");
	sem_post(sem); // "unlock" by incrementing the sem value to 1, allowing
	 // the child to 'unlock' and proceed forward...
	wait(NULL);

/* Now the child has printed and exited */
	printf("third\n");
	sem_close(sem);
	sem_unlink(SEM_NAME);
}
