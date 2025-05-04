#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	FILE *fp;
	char buf[80];

	fp = popen("date", "r");
	if (!fp) {
		perror("popen");
		exit(1);
	}
	fgets(buf, 80, fp);
	pclose(fp);
	printf("%s", buf);

	exit(0);
}
