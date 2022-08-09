#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int	s4_audio();

int s3_engine()
{
	printf("engine: in %s:%s() now...\n", __FILE__, __func__);
	s4_audio();

	return 0;
}
