#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../hdr.h"

int s3_engine()
{
	printf("engine: in %s:%s() now...\n", __FILE__, __func__);
	g ++;
	SHOW_VAR(g);
	s4_audio();

	return 0;
}
