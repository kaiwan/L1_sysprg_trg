#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	FILE *fp;
	char buf[80];
	int i;

	for (i = 0; i < 5; i++) {
		fp = popen("date", "r");
		if (!fp) {
			perror("popen");
			exit(1);
		}
		fgets(buf, 80, fp);
#if 0
		pclose(fp);
#endif
		printf("%s\n", buf);
	}
	while (1)
		pause();
}

/*
Without the pclose(), the fork-ed children (via popen) become zombies!

$ ps
$ ps -l
F S   UID   PID  PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000 28182  4263  0  80   0 -  5818 wait   pts/4    00:00:00 bash
0 S  1000 28451 28182  0  80   0 -  1086 pause  pts/4    00:00:00 popen_tst
0 Z  1000 28452 28451  0  80   0 -     0 -      pts/4    00:00:00 sh <defunct>
0 Z  1000 28454 28451  0  80   0 -     0 -      pts/4    00:00:00 sh <defunct>
0 Z  1000 28456 28451  0  80   0 -     0 -      pts/4    00:00:00 sh <defunct>
0 Z  1000 28458 28451  0  80   0 -     0 -      pts/4    00:00:00 sh <defunct>
0 Z  1000 28460 28451  0  80   0 -     0 -      pts/4    00:00:00 sh <defunct>
4 R  1000 28465 28182  0  80   0 -  7834 -      pts/4    00:00:00 ps
$ 
*/
