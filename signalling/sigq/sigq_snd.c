/*
 * sigq_snd.c
 * 
 * History:
 *
 * Author(s) : 
 * Kaiwan N Billimoria
 *  <kaiwan -at- kaiwantech -dot- com>
 *
 * License(s): [L]GPL
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

/*---------------- Macros -------------------------------------------*/


/*---------------- Typedef's, constants, etc ------------------------*/


/*---------------- Functions ----------------------------------------*/


int main (int argc, char **argv)
{
	union sigval sv;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s pid-to-send-to value-to-send[int]\n", argv[0]);
		exit (1);
	}

	sv.sival_int=atoi(argv[2]);
	if (sigqueue(atol(argv[1]), SIGRTMIN+3, sv) == -1) {
	//if (sigqueue(atol(argv[1]), SIGINT, sv) == -1) {
		perror("sigqueue failed");
		exit (1);
	}
	exit (0);
}

/* vi: ts=4 */
