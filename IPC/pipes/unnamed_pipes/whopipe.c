/* whopipe. c
 * 
 * Implementation of who | wc -l
 */

#define LINELENGTH 132
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

int main()
{
	char buf[LINELENGTH+1];
	char *s=buf;
	FILE *fp1, *fp2;

/* Get access to the standard output of the LHS cmd of the pipe */
	if( (fp1=popen( "who", "r" )) == NULL ) {
 		fprintf(stderr, "Error opening pipe\n");
	    	exit(1);
	} /* if */

/* Get access to the standard input of the RHS cmd of the pipe */
	if( (fp2=popen( "wc -l", "w")) == NULL ) {
   		fprintf(stderr,"Error opening pipe\n");
	    	exit(1);
	} /* if */
/* Send stdout of LHS cmd to stdin of the RHS command */
	while( fgets(s,LINELENGTH,fp1) != NULL )
		fputs(s, fp2);
	
	pclose(fp1);
	pclose(fp2);
	exit (0);
} /* main - whopipe.c */

