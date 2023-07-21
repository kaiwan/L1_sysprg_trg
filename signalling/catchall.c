// catchall.c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <string.h>

static char prgname[128];

static void signal_handler(int signum)
{
	fprintf(stderr, "%s: caught sig %2d\n", prgname, signum);
}

int main(int argc, char **argv)
{
	struct sigaction act;
	int i;

	memset(prgname, 0, sizeof(prgname));
	strncpy(prgname, argv[0], 127);

	memset(&act, 0, sizeof(act));
	act.sa_handler = signal_handler;
	act.sa_flags = SA_RESTART;
	sigemptyset(&act.sa_mask);

	printf("%s:PID %d\n", argv[0], getpid());
	for (i=1; i<=64; i++) {
		//printf("Trapping into signal %d\n", i);
		if (sigaction(i, &act, 0) == -1) {
			fprintf(stderr, "signal # %2d ", i);
			perror(" *** sigaction failed");
		}
	}

	printf("%s: awating signals ...\n", prgname);
	while (1)
		pause();
}
