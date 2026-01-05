/*
 * sleepsafe.c
 * 
 * Author :  Kaiwan N Billimoria, kaiwanTECH
 * License(s): MIT
 */
extern int errno;

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

int sleepsafe(ssize_t sec, ssize_t nsec, int verbose)
{
	struct timespec req, rem;

	req.tv_sec = sec;
	req.tv_nsec = nsec;
	while ((nanosleep(&req, &rem) == -1) && (errno == EINTR)) {
		if (verbose)
			printf("nanosleep interrupted: rem time: %07lu.%07lu\n",
				rem.tv_sec, rem.tv_nsec);
		req = rem;
	}
	return 0;
}

int main(int argc, char **argv)
{
	ssize_t sec, nsec;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s sleep_seconds sleep_nanoseconds\n", argv[0]);
		exit(1);
	}
	sec = atoi(argv[1]);
	nsec = atoi(argv[2]);

	printf("sleep for %ld.%ld s now...\n", sec, nsec);
	sleepsafe(sec, nsec, 1);
	//sleepsafe(3,5*100*1000*1000); // 3.5s

	exit(0);
}
