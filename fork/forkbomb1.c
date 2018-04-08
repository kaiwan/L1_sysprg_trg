// forkbomb
#include "../common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

const int BLK_MMAP = 512 * 1024;
const int BLK_HEAP = 5 * 1024;

int main(int argc, char **argv)
{
	struct rlimit rlim;
	int stress=0, i;
	void *pmmap, *pheap;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s {num-times-to-fork} {stress-it [0|1]}\n", argv[0]);
		exit(1);
	}
	if (atoi(argv[2]) == 1)
		stress=1;

	if (prlimit(0, RLIMIT_NPROC, 0, &rlim) == -1)
             FATAL("prlimit failed\n");
	printf("Resource limit RLIMIT_NPROC :: hard: %ld soft: %ld\n",
			rlim.rlim_max, rlim.rlim_cur);

	for (i = 0; i < atoi(argv[1]); i++) {
		switch (fork()) {
		case -1 : FATAL("fork failed: iteration #%d\n", i);
		case 0 :  // Child
			pmmap = malloc(BLK_MMAP);
			pheap = malloc(BLK_HEAP);
			if (stress) {
				memset(pmmap, 0xff, BLK_MMAP);
				memset(pheap, 0xee, BLK_HEAP);
			}
			// it's important to keep the child alive
			pause();
		default : // Parent
			if (i && !(i%1000))
				printf("%6d ...\n", i);
		}
	}
	exit(0);
}
