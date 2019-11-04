/*
 * savedset_ids.c
 * Build an SUID executable, owned by root;
 * have a "regular" user execute it and print RUID, EUID.
 * Then switch to your "regular" non-root ID and later switch back to 
 * root perms as required.
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * License: MIT
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <assert.h>

#define SHOWIDS() do { \
  printf ("Now: RUID=%d EUID=%d\n"  \
          "     RGID=%d EGID=%d\n", \
			getuid(), geteuid(),    \
			getgid(), getegid());   \
} while (0)

int main(int argc, char **argv)
{
	uid_t orig_euid;
	char *cmd = malloc(PATH_MAX);

	assert(cmd);
	printf("First make sure that %s is an SUID-root binary\n", argv[0]);
	snprintf(cmd, 128, "ls -l %s", argv[0]);
	system(cmd);
	SHOWIDS();

	if (0 != geteuid()) {
		fprintf(stderr, "%s: not a setuid-root executable, aborting now ...\n"
			" [Tip: do: sudo chown root:root %s ; sudo chmod u+s %s\n"
			" and rerun].\n"
			, argv[0], argv[0], argv[0]);
		free(cmd);
		exit(1);
	}
	printf("1. Ok, we're effectively running as root! (euid==0)\n");

	// save original RUID; i.e the saved set-uid
	orig_euid = geteuid();

	printf("2. Changing Effective UID back to my \"original\" UID now...(and going to sleep for 10s...)\n");
	if (-1 == seteuid(getuid())) {
		perror("seteuid [1] failed"); 
		free(cmd);
		exit(1);
	}
	SHOWIDS();

	sleep(10);

	printf("3. Now reverting to my saved \'superior\' privileges...\n");
	if (-1 == seteuid(orig_euid)) {
		perror("seteuid [2] failed"); 
		free(cmd);
		exit(1);
	}
	SHOWIDS();

	printf("4. Exiting ...\n");
	free(cmd);
	exit(0);
}
