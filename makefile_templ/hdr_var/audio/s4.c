#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../hdr.h"

int s4_audio()
{
	printf("audio: hey in %s:%s() now...\n", __FILE__, __func__);
	return 0;
}
