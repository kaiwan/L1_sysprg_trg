#include <stdio.h>
#include <unistd.h>
#include <sched.h>

main()
{
	int min, max;

	min = sched_get_priority_min(SCHED_FIFO);
	max = sched_get_priority_max(SCHED_FIFO);
	printf("SCHED_FIFO: min=%d max=%d\n", min, max);

	min = sched_get_priority_min(SCHED_RR);
	max = sched_get_priority_max(SCHED_RR);
	printf("SCHED_RR: min=%d max=%d\n", min, max);

	min = sched_get_priority_min(SCHED_OTHER);
	max = sched_get_priority_max(SCHED_OTHER);
	printf("SCHED_OTHER: min=%d max=%d\n", min, max);
}
