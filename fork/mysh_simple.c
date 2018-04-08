/* 
 * mysh_simple.c
 *
 * "my simple shell"
 * o -v : show child termination status info
 * +o -D : MYDEBUG is ON, else OFF
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 * MIT License.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdarg.h>

#define LEN		128
#define PROMPT	"mysh> "
#define ON	1
#define OFF	0

static int MYDEBUG = OFF;

static void Dprint(const char *fmt, ...)
{
	va_list ap;
	if (MYDEBUG == OFF)
		return;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

int main(int argc, char **argv)
{
	char cmd[LEN];
	char *exit_str = "q";

	if ((argc > 1) && (strcmp(argv[1], "-D") == 0))
		MYDEBUG = ON;

	while (1) {
		printf("%s", PROMPT);
		fflush(stdout);

		/* fgets is safe; gets is not. (Btw, getline(3) could be even better!) */
		fgets(cmd, LEN, stdin);
		cmd[strlen(cmd) - 1] = '\0';	/* remove trailing \n */
		Dprint("cmd: len=%d, cmd=%s\n", strlen(cmd), cmd);

		if (cmd[0] == 0)	/* try to take care of LF */
			continue;

		if (strncmp(cmd, exit_str, strlen(cmd)) == 0) {
			printf("q pressed.\n");
			exit(0);
		}

		switch (fork()) {
		case -1:
			perror("fork failed");
			break;
		case 0:	// Child
			if (execlp(cmd, cmd, (char *)0) == -1) {
				perror("exec failure");
				exit(1);
			}
			/* code never reaches here.. */
		default:	// Parent
			if (wait(0) == -1)
				perror("wait"), exit(1);
		}
	}			// while
	/* code never reaches here.. */
} // main()
