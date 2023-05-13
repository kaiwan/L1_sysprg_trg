#include <stdio.h>
#include <unistd.h>
#include <assert.h>

static void foo(void)
{
#if 1
	double pi, q; // hey, locals are uninitialized! 
#else
	double pi = 3.14, q;
#endif
	int factorx = 3;

#if 1
	// expect / assume pi to be 3.14
	assert(pi == 3.14);
#endif
	q = pi*factorx; /* possible UMR bug ! the assertion will prevent it
						from occuring... */
	printf("pi=%.2f, q=%.2f\n", pi, q);
}

int main()
{
	foo();
	return 0;
}
