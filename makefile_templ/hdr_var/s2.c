#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hdr.h"

int s3_engine();

int s2_a()
{
	printf("s2: in %s:%s() now...\n", __FILE__, __func__);
	s3_engine();

	return 0;
}
