#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>

static char arr[5];

void foo(int num)
{
	int i;
	assert(num == sizeof(arr));

	for (i=0; i<=num; i++)
		arr[i] = 'x';
}

int main()
{
	int loc;

	printf("Hello, assert world\n");
	assert(loc == 0);
	foo(6);
	exit(0);
}
