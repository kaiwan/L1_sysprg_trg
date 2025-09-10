// main.c
#include <stdio.h>

// forward declaration; usually kept in a header
int add(int, int);

int main()
{
	printf("Sum: %d\n", add(3, 5));
	return 0;
}

/*
 What if it fails like this:
$ ./main 
./main: error while loading shared libraries: libmymath.so: cannot open shared object file: No such file or directory

Then do this:
$ export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:.
$ ./main 
Sum: 8

Works.
*/ 
