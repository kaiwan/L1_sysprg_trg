/* envp.c:
 *
 * Demonstrate explicit environment passing with execle() .
 * Execute the printenv command after changing the environment
 * value of the TERM variable to dialup.
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

extern char **environ; /* the environment */
#define MAXENV	4096

int main(int argc, char **argv)
{
	int i, j;
	static char *envp[MAXENV];	/* Pointers to environment */
	char *s = "TERM=dialup";

	for (i = 0; environ[i] != NULL; i++) {
		envp[i] = environ[i];
	}
	envp[i] = NULL;

	/* Search for TERM variable in environment.
	 * Note the use of the strncpy (as opposed to strcpy) : this is for 
	 * security, helping prevent buffer overflow attacks.
	 */
	j = 0;
	while ((strncmp("TERM=", envp[j], 5) != 0) && (j < (i - 1)))
		j++;
	if (j >= (i - 1)) {
		fprintf(stderr, "%s: env variable TERM not found. Aborting..\n",
			argv[0]);
		exit(1);
	}
	envp[j] = s;

	/*
	   gcc gives: envp.c:42: warning: missing sentinel in function call
	   Solution:
	   From the man page:
	   "The list of arguments must be terminated by a NULL pointer, and, 
	   since these are variadic functions, this pointer must be 
	   cast (char *) NULL"
	 */
	if (execle("/usr/bin/printenv", "printenv", (char *)0, envp) < 0) {
		perror(argv[0]);
		exit(1);
	}			/* if */
	exit(0);		// just to get rid of the compiler warning 
	// ("warning: control reaches end of non-void function")
}
