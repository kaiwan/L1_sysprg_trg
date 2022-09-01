#include <stdio.h>
#include <unistd.h>
#include <assert.h>

static void foo(void)
{
#if 1
	double r, q; // hey, locals are uninitialized! 
#else
	double r = 3.14, q;
#endif
	int factorx = 3;

#if 1
	// expect / assume r to be 3.14
	assert(r == 3.14);
#endif
	q = r*factorx; // possible UMR bug ! the assertion will prevent it
	// from occuring...
	printf("r=%.2f, q=%.2f\n", r, q);
}

int main()
{
	foo();
	return 0;
}
