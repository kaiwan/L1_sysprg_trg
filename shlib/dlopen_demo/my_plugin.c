#include <stdio.h>

// A simple function we want to load dynamically
void print_message(const char *name)
{
	printf
	    ("Hello %s, this message comes dynamically from the dlopen()'ed shared library!\n",
	     name);
}
