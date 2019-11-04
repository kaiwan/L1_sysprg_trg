/* pipe.c
 * Illustrate the use of the pipe() and fork() calls. 
 * Here, the parent "sends a message" to the child process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define	MAX		80

int main(int argc, char **argv)
{
	int pid;		/* Value returned by fork() call	*/
	int n;			/* Return value from read() call	*/
	int fd[2];		/* Pipe file descriptor array		*/
	char line[MAX], buffer[MAX];	/* buffers for user input	*/
						
	fprintf(stderr, "Now creating pipe..\n");
	if( pipe( fd ) !=0 ) {
		perror(argv[0]);
		exit (1);
	}

	switch(pid=fork()) {
		case -1: perror(argv[0]);
			 exit (1);

		case 0 :	// Child process
	   		 close(fd[1]);			/* Close write end of pipe  */
   	  		 n = read( fd[0], buffer, MAX);
			 if( n==-1 ) {
				perror("child read"); close(fd[0]); exit(1);
			 }
   	  		 buffer[n] = '\0';

   	  		 printf("Child: your line was: \"%s\"\n", buffer);
   	  		 close(fd[0]);			/* Close read end of pipe  */
   	  		 exit( 0 );

		default: 	// Parent process
	   		close(fd[0]);			/* Close read side of pipe */
 		  	printf( "Enter line: " );	/* Accept line of input	*/	
   	  		fgets(line,MAX,stdin);						

  			printf( "Parent: Writing your line to child with pipe...\n" );
  	  		n=write( fd[1], line, strlen(line) ); /* Write line to pipe */
			if( n==-1 ) {
				perror("parent write"); close(fd[1]); exit(1);
			}
  		 	close(fd[1]);			/* Close write side of pipe  */
   	  		wait(0);		 	/* Wait for child to exit    */
   	}
	exit (0);	/* just to avoid compiler warning */
} /* main - pipe1.c */

