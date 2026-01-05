// initial: courtesty ChatGPT-4
// File: named_semaphore_example.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SEM_NAME "/my_named_semaphore"  // actually here under /dev/shm

void write_to_file(const char *msg)
{
	FILE *f = fopen("output.txt", "a");
	if (f == NULL) {
		perror("fopen");
		exit(1);
	}
	fprintf(f, "%s\n", msg);
	fclose(f);
}

int main(void)
{
	// start afresh
	sem_unlink(SEM_NAME);
	unlink("output.txt");
	
	/* Create a named semaphore and initialize it to 1
	 * 1 => it's a binary sem or mutex usage and set to 'unlocked' state
	 */
	sem_t *sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
	if (sem == SEM_FAILED) {
		perror("sem_open");
		exit(1);
	}

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		sem_close(sem);
		sem_unlink(SEM_NAME);
		exit(1);
	}

	/* BOTH parent & child run the code below in parallel
	 *
	 * Diagramatically:
	 * Y-axis is the semaphore value
	 *
	 * P/C* calls sem_wait()

	1 |________________                        ____ [...repeats...]
	  |               |LOCK                    ^
  	  |               | <...work...>           |
	0 |               v______________sem_post()|UNLOCK
	  +--------------------------------------------------------------> t
	 *
	 * P/C : Parent or Child process
	 */
	for (int i = 0; i < 5; i++) {
		sem_wait(sem);	/* LOCK: 
		                 *  if sem value is +ve, decrement & proceed
				 *  if sem value is 0, *wait* until peer process 'posts'/increments
				 *   the value to 1, i.e. until the peer process 'unlocks' it!
				 */
		// By the time we're here, one of us - parent or child - 'owns' the 'lock'
		if (pid == 0)
			write_to_file("Child writing");
		else
			write_to_file("Parent writing");

		sem_post(sem);	// UNLOCK: increment the sem value
		usleep(100000);	// Sleep 100ms to mix up scheduling
	}

	if (pid > 0) {
		wait(0);	// Wait for child
		sem_close(sem);
		sem_unlink(SEM_NAME);	// Cleanup
	} else {
		sem_close(sem);
	}

	return 0;
}
