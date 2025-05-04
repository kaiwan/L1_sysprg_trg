/* pipe-dup2.c 
 *
 * Use the dup2() system call to implement the shell command
	   who | wc - l
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd[2];

	if (pipe(fd) == -1) {
		perror("dup : pipe"), exit(1);
	}

	switch (fork()) {
	case -1:
		perror("dup2: fork");
		exit(1);

	case 0:		// Child process
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);

		if (execl("/usr/bin/wc", "wc", "-l", (char *)0) == -1) {
			perror("dup: exec1"), exit(1);
		}

	default:		// Parent process
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);

		if (execlp("who", "who", (char *)0) == -1) {
			perror("dup: execlp"), exit(1);
		}
	}			// switch
	exit(0);		/* just to avoid compiler warning */
}
