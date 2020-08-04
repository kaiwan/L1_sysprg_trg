/*
 * mt_fork_exec.c
 *
 * Call fork() within a process that has multiple threads alive.
 * Duplicates only the thread that invokes fork in the new child.
 * Here, we demo this by having the "child" process (a copy of the
 * thread that called fork()), exec the PDF reader app (evince)!
 *
 * (c) kaiwanTECH
 * License: MIT
 */
#define _POSIX_C_SOURCE    200112L	/* or earlier: 199506L */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LAST_COL	79

// Thread p2
// write() 's "2" to stderr
void *thrd_p2(void *msg)
{
	printf("%s(): will terminate in 2s...\n", __func__);
	sleep(2);
	pthread_exit(NULL);
}

/* Thread p3
 * Parameter: the PDF file to view
 */
void *thrd_p3(void *msg)
{
	pid_t n;
	char *fname = (char *)msg;

	if (!fname) {
		fprintf(stderr, "%s(): PDF-file-to-view absent?\n", __func__);
		exit(1);
	}
	printf("%s(): PID %d...fname=%s\n", __func__, getpid(), fname);

	printf("+++ Worker thread 2: fork() in 3 seconds...\n");
	sleep(3);
	switch (n = fork()) {
	case -1:
		perror("fork failed");
		exit(1);
	case 0:		// Child
		printf("In child: PID %d. Exec-ing evince now ...\n", getpid());
		if (execl
		    ("/usr/bin/evince", "evince PDF reader", fname,
		     (char *)0) < 0) {
			perror("execl failed!");
			exit(1);
		}
		exit(0);
	default:
		printf("worker 2: waiting for child...\n");
		printf("worker 2: child %d died...\n", wait(0));
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	pthread_t p2, p3;
	int fd, r;
	void *ret;

	if (argc == 1) {
		fprintf(stderr, "Usage: %s PDF-file-to-view\n", argv[0]);
		exit(1);
	}
	// Ensure this file is present!
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr,
			"%s: PDF-file-to-view \"%s\" not existing or unreadable?\n",
			argv[0], argv[1]);
		perror("File open failed");
		exit(1);
	}
	close(fd);

	// Create 2 threads (main p1 is parent..)
	printf("p1 (the main() thread): now creating pthread p2..\n");
	r = pthread_create(&p2,	// thread id
			   NULL,	// thread attributes (use default)
			   thrd_p2,	// function to execute
			   (void *)argv[1]);	// argument to function
	if (r)
		perror("pthread creation"), exit(1);

	printf("p1 (the main() thread): now creating pthread p3..\n");
	r = pthread_create(&p3,	// thread id
			   NULL,	// thread attributes (use default)
			   thrd_p3,	// function to execute
			   (void *)argv[1]);	// argument to function
	if (r)
		perror("pthread creation"), exit(1);

	printf("p1: threads p2 and p3 created...\n");
	fflush(stdout);

	// Have main join on child threads
	printf("main: join-ing ...\n");
	if (pthread_join(p2, &ret) != 0)
		perror("pthread join"), exit(1);
	printf("p1: p2 has joined up\n");
	if (pthread_join(p3, &ret) != 0)
		perror("pthread join"), exit(1);
	printf("p1: p3 has joined up\n");

	exit(0);
}
