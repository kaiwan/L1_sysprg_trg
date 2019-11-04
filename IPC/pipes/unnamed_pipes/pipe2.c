/* 
 * pipe2.c:	Illustrate the use of the pipe() and fork() calls. 
 *
 * In addition, we test the case of the child dying abnormally;
 * this results in SIGPIPE being sent to the parent (as the read end
 * is now closed)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

static void pipe_broke( int sig )
{
	printf("In SIGPIPE signal handler::pipe_broke: signal %d\n",sig);
}

int main(int argc, char **argv)
{
	int child_exit_flag=0;
	struct sigaction act;
	int pid;		/* Value returned by fork() call	*/
	int n;			/* Return value from read() call	*/
	int fd[2];		/* Pipe file descriptor array		*/
	char line[80], buffer[80];	/* buffers for user input	*/

	if (argc > 2) {
		fprintf(stderr, "Usage: %s child_exit_flag=[0]|1\n\
where child_exit_flag = 0 : normal execution (default)\n\
child_exit_flag = 1 : child process dies immd. (resulting in SIGPIPE to \
parent)\n", argv[0]);
		exit (1);
	}
	if (argc == 2 && atoi(argv[1]) == 1)
		child_exit_flag=1;

	act.sa_handler = pipe_broke;
	sigemptyset (&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGPIPE, &act, 0) == -1) {
		perror("sigaction failed");
		exit (1);
	}

	fprintf(stderr, "Now creating pipe..\n");
	if( pipe( fd ) !=0 ) {
		perror(argv[0]);
		exit (1);
	}

	if( (pid=fork()) <0) {
		perror(argv[0]);
		exit (1);
	}

	if(pid != 0) {		/* Parent process */
		//sleep (1);
		/* sync w/ child */
		if (child_exit_flag == 1)
			if (wait (0) == -1)
				perror("parent: wait failed"), exit (1);

	   close(fd[0]);		/* Close read side of pipe */
   	   printf( "Enter line: " );	/* Accept line of input	*/	
   	   fgets(line,80,stdin);						

  	   printf( "Writing your line to child with pipe...\n" );
  	   n=write( fd[1], line, strlen(line) ); /* Write line to pipe */
	   if( n == -1 ) {
		perror("parent:write failed");
	   }

	   printf("write: %d bytes written\n",n);
   	   close(fd[1]);			/* Close write side of pipe  */
   	   wait(0);			 	/* Wait for child to exit    */
   	} /* in parent */
	else {	  /* in child */
		if (1 == child_exit_flag)
		/* Delibrately have the child die immd. to test the situation
		* when the read end is shut and the writer is attempting 
		* to write data; the writer will receive SIGPIPE
		*/
			exit( 1 );

	   close(fd[1]);			/* Close write end of pipe  */
   	   n = read( fd[0], buffer, 80);
   	   buffer[n] = '\0';

   	   fprintf(stderr, "Child: your line was %s\n", buffer);
   	   close(fd[0]);			/* Close read end of pipe  */
   	   exit( 0 );
   	}
	exit (0);	/* just to avoid compiler warning */
} /* main - pipe2.c */

