#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
int n;
int main (int argc, char **argv)
{
	pid_t ret;
	ret = fork();
	switch (ret) {
case -1: perror("fork failed");
  exit(1);
case 0 : // Child
printf("Child: ret=%d pid=%d\n", ret, getpid());
printf("Child sleeping for 5s...\n");
sleep(5);
exit(0);
default : // Parent
printf("Parent: ret=%d pid=%d\n", ret, getpid());
printf("Parent sleeping for 8s...\n");
sleep(8);
 exit(0);
}
}
