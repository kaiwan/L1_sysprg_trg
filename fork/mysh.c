/* 
 * mysh.c
 *
 * "my simple shell"
 * o -v : show child termination status info
 * +o -D : MYDEBUG is ON, else OFF
 *
 * Memory checking with valgrind..
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * MIT License.
 */

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define LEN		128
#define MAXARGS 1024
#define PROMPT	"mysh> "
#define SEP     " "

#define ON	1
#define OFF	0

static int MYDEBUG = OFF;
#define SHOWARGS() do { \
	int i;	\
	if (MYDEBUG) {	\
		for (i=0;i<MAXARGS;i++)	\
			printf("newargs[%d]=%p\n\
orig_newargs[%d]=%p\n", \
				i, newargs[i],\
				i, orig_newargs[i]); \
		}	\
} while(0)

static void Dprint(const char *fmt, ...)
{
	va_list ap;
	if (MYDEBUG == OFF)
		return;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

static inline void free_args(char **args)
{
	int i;
	for (i = 0; i < MAXARGS; i++) {
		if (args[i])
			free(args[i]);
	}
}

static void displayChildStatus(int stat, pid_t cpid)
{
	printf("\nChild %d just died!\n", cpid);
	if (WIFEXITED(stat))
		printf(" exited normally, exit status: %d\n",
		       WEXITSTATUS(stat));
	if (WIFSIGNALED(stat))
		printf(" exited abnormally, signal killed with: %d\n",
		       WTERMSIG(stat));
	if (WIFSTOPPED(stat))
		printf(" stopped; stop signal: %d\n", WSTOPSIG(stat));
	if (WCOREDUMP(stat))
		printf(" dumped core.\n");
	if (WIFCONTINUED(stat))
		printf(" continued.\n");
}

static void show_resusage(struct rusage *rusage)
{
	printf("Resource usage stats ::\n"
	       " time in user   mode       : %6ld:%6ld [s:us]\n"
	       " time in kernel mode       : %6ld:%6ld [s:us]\n"
	       " max RSS                   : %9ld KB\n"
	       " minor faults (no IO)      : %9ld\n"
	       " major faults (IO)         : %9ld\n"
	       " #times fs performed i/p   : %9ld\n"
	       " #times fs performed o/p   : %9ld\n"
	       " # voluntary ctx switches  : %9ld\n"
	       " # involuntary ctx switches: %9ld\n", 
		   rusage->ru_utime.tv_sec,
	       rusage->ru_utime.tv_usec, rusage->ru_stime.tv_sec,
	       rusage->ru_stime.tv_usec, rusage->ru_maxrss, rusage->ru_minflt,
	       rusage->ru_majflt, rusage->ru_inblock, rusage->ru_oublock,
	       rusage->ru_nvcsw, rusage->ru_nivcsw);
}

int main(int argc, char **argv)
{
	char cmd[LEN];
	/* 
	 * IMPORTANT NOTE:-
	 *
	 * The strtok routines (below), break up and *change* the pointers.
	 *
	 * From strtok man page:
	 *--snip--
	 BUGS
	 Avoid using these functions.  If you do use them, note that:

	 These functions modify their first argument.

	 These functions cannot be used on constant strings.
	 *--snip--
	 *
	 * So, attempting to free the pointers on termination results in "free
	 * errors" being caught (by glibc 2.4 and valgrind).
	 * So we save the pointers in orig_newargs and free these, not newargs.
	 * (Use the SHOWARGS macro (turn debugging ON), to clearly see this).
	 */
	char *newargs[MAXARGS];
	char *orig_newargs[MAXARGS];
	int i, stat = 0;
	char *exit_str = "q";
	pid_t cpid;
	struct rusage *rusage = malloc(sizeof(struct rusage));

	if ((argc > 1) && (strcmp(argv[1], "-D") == 0))
		MYDEBUG = ON;

	for (i = 0; i < MAXARGS; i++) {
		newargs[i] = malloc(LEN);
		if (newargs[i] == NULL) {
			fprintf(stderr, "%s: no memory for newargs[%d]..",
				argv[0], i);
			exit(1);
		}
		orig_newargs[i] = newargs[i];
	}
	SHOWARGS();

	while (1) {
		SHOWARGS();

		printf("%s", PROMPT);
		fflush(stdout);

		/* fsgets is safe; gets is not */
		fgets(cmd, LEN, stdin);
		cmd[strlen(cmd) - 1] = '\0';	/* remove trailing \n */
		Dprint("cmd: len=%d cmd=%s\n", strlen(cmd), cmd);

		if (cmd[0] == 0)	/* try to take care of LF */
			continue;

		if (strncmp(cmd, exit_str, strlen(cmd)) == 0) {
			printf("q pressed.\n");
			free_args(orig_newargs);
			exit(0);
		}

		/* Tokenize..
		 * warning : strtok is not thread-safe (and, see comment above)..
		 */
		newargs[0] = strtok(cmd, SEP);
		i = 1;
		while ((newargs[i++] = strtok(NULL, SEP))) ;

		if ((i - 2) > MAXARGS) {
			/*printf("# args (i-1)=%d\n", i-1); */
			fprintf(stderr,
				"%s: Too many arguments (max is %d), aborting command.\n",
				argv[0], MAXARGS);
			continue;
		}

		if (MYDEBUG) {
			i = 0;
			while (newargs[i]) {
				printf("\nnewargs[%d]=%s\n", i, newargs[i]);
				i++;
			}
			printf("\n");
		}

		switch (fork()) {
		case -1:
			perror("fork failed");
			break;
		case 0:	// Child
			free_args(orig_newargs);
			if (execvp(newargs[0], newargs) == -1) {
				perror("exec failure");
				exit(1);
			}
			/* code never reaches here.. */
		default:	// Parent
			// WUNTRACED => can't be spoofed into unblocking wait on child 
			// getting stopped and not dead
			if ((cpid = wait3(&stat, WUNTRACED, rusage)) == -1)
				perror("wait"), exit(1);

			// Show child termination stat & resource usage info
			displayChildStatus(stat, cpid);
			show_resusage(rusage);

			//free (rusage);
		}
	}			// while
	/* code never reaches here.. */
}				// main()

/*
Running valgrind on this (mysh_dbg - debug ver) with verbose switch on also clearly
shows how the pointers are changing.

$ make && valgrind --leak-check=full -v ./mysh_dbg
make: Nothing to be done for `all'.
==9349== Memcheck, a memory error detector.
==9349== Copyright (C) 2002-2006, and GNU GPL'd, by Julian Seward et al.
==9349== Using LibVEX rev 1658, a library for dynamic binary translation.
==9349== Copyright (C) 2004-2006, and GNU GPL'd, by OpenWorks LLP.
==9349== Using valgrind-3.2.1, a dynamic binary instrumentation framework.
==9349== Copyright (C) 2000-2006, and GNU GPL'd, by Julian Seward et al.
==9349==
--9349-- Command line
--9349--    ./mysh_dbg
--9349-- Startup, with flags:
--9349--    --leak-check=full
--9349--    -v
--9349-- Contents of /proc/version:
--9349--   Linux version 2.6.16.21-0.8-smp (geeko@buildhost) (gcc version 4.1.0 (SUSE Linux)) #1 SMP Mon Jul 3 18:25:39 UTC 2006
--9349-- Arch and hwcaps: X86, x86-sse1-sse2
--9349-- Valgrind library directory: /usr/local/lib/valgrind

--snip--

$ ps
#(our comment)--vvvvvvvvv------------------------vvvvvvvvv--------
--9349-- REDIR: 0x40B5BB0 (memchr) redirected to 0x4022420 (memchr)
#(our comment)--^^^^^^^^^------------------------^^^^^^^^^--------
#(our comment) can see how strtok has changed the pointer

--9349-- REDIR: 0x40B65B0 (memcpy) redirected to 0x4022C20 (memcpy)
--9349-- REDIR: 0x40B48F0 (strcmp) redirected to 0x4022300 (strcmp)
--9351-- REDIR: 0x40B4780 (index) redirected to 0x4022060 (index)
--9351-- REDIR: 0x40B5080 (strncmp) redirected to 0x4022290 (strncmp)
--9351-- REDIR: 0x40B6F30 (strchrnul) redirected to 0x40225E0 (strchrnul)
  PID TTY          TIME CMD
 8289 pts/1    00:00:02 bash
 9349 pts/1    00:00:00 memcheck
 9351 pts/1    00:00:00 ps
$ ls -l -F
--9352-- REDIR: 0x40B4780 (index) redirected to 0x4022060 (index)
--9352-- REDIR: 0x40B5080 (strncmp) redirected to 0x4022290 (strncmp)
--9352-- REDIR: 0x40B6F30 (strchrnul) redirected to 0x40225E0 (strchrnul)
total 72
-rwxr-xr-x 1 kaiwan users  7497 2006-12-06 13:29 leaky*
-rw-r--r-- 1 kaiwan users  1137 2006-12-06 13:24 leaky.c
-rwxr-xr-x 1 kaiwan users  9842 2006-12-06 13:29 leaky_dbg*
-rw-r--r-- 1 kaiwan users   490 2006-12-07 11:35 Makefile
-rwxr-xr-x 1 kaiwan users  8700 2006-12-07 12:30 mysh*
-rw-r--r-- 1 kaiwan users  2615 2006-12-06 14:17 mysh2.c
-rw-r--r-- 1 kaiwan users  2671 2006-12-07 12:30 mysh.c
-rwxr-xr-x 1 kaiwan users 11636 2006-12-07 12:30 mysh_dbg*
-rw-r--r-- 1 kaiwan users  1919 2006-12-07 11:39 simp.c
-rwxr-xr-x 1 kaiwan users  7269 2006-12-07 11:39 simp_dbg*
$ q
q pressed.
--9349-- REDIR: 0x40AFF00 (free) redirected to 0x4020F65 (free)
--9349-- REDIR: 0x40B60B0 (memset) redirected to 0x4022540 (memset)
==9349==
==9349== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 3 from 1)
--9349--
--9349-- supp:    3 Fedora-Core-5-hack3-ld24
==9349== malloc/free: in use at exit: 0 bytes in 0 blocks.
==9349== malloc/free: 5 allocs, 5 frees, 640 bytes allocated.
==9349==
==9349== All heap blocks were freed -- no leaks are possible.
--snip--
$ 

*/
