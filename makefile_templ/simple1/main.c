#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Prototypes
int s2_a();


int main(int argc, char **argv)
{
	printf("Hey there! in %s:%s() now...\n", __FILE__, __func__);
	s2_a();

	exit(EXIT_SUCCESS);
}
