/* Code Listing 3.11:
   Creating and using a named POSIX semaphore to control the timing of parent/child execution
   https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/IPCSems.html#cl3-11
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>		/* For O_* constants */
#include <sys/stat.h>		/* For mode constants */
#include <semaphore.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SEM_NAME "/opencsf_sem"

int main(void)
{
/* Create and open the semaphore */
	sem_t *sem =
	    sem_open(SEM_NAME, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
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

/* Child process: wait for semaphore, print "second", then exit */
	if (child_pid == 0) {
		sem_wait(sem);
		printf("second\n");
		sem_close(sem);
		return 0;
	}

	/* Parent prints then posts to the semaphore and waits on child */
	printf("first\n");
	sem_post(sem);
	wait(NULL);

/* Now the child has printed and exited */
	printf("third\n");
	sem_close(sem);
	sem_unlink(SEM_NAME);
}
