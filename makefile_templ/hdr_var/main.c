#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hdr.h"

// Prototypes
//int s2_a();

int g = 5;

int main(int argc, char **argv)
{
	printf("Hey there! in %s:%s() now...\n", __FILE__, __func__);
	SHOW_VAR(g);
	s2_a();

	exit(EXIT_SUCCESS);
}
