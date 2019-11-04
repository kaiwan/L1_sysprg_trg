/* whichexit.c */
#include <errno.h> 
#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

void *whichexit(void *arg) { 
	int n; 
	int np1[1]; 
	int *np2; 
	char s1[10]; 
	char s2[] = "I am done"; 
	n = 3; 
	np1[0] = n; 
	np2 = (int *)malloc(sizeof(int *)); 
	*np2 = n; 
	strcpy(s1, "Done"); 
	return(NULL); 
} 

int main(void)
{
	pthread_t tid;
	int error;

	if ((error = pthread_create (&tid, NULL, whichexit, 0))) {
		fprintf (stderr, "Failed to create thread: %s\n", strerror(error));
		return 1;
    }
	pthread_join (tid, NULL);
	return 0;
}

