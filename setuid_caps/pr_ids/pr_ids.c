/*
 * pr_ids.c
 * Build an SUID executable, owned by root;
 * have a "regular" user execute it and print RUID, EUID.
 * 
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * License: MIT
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define SHOWIDS() do { \
  printf ("Now: RUID=%d EUID=%d\n"  \
          "     RGID=%d EGID=%d\n", \
			getuid(), geteuid(),    \
			getgid(), getegid());   \
} while (0)

int main(int argc, char **argv)
{
	SHOWIDS();
	if (0 == geteuid()) {
		printf("%s now effectively running as root! ...\n", argv[0]);
		sleep(1);
	}
	exit(0);
}
