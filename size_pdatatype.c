#include <stdio.h>
#include <limits.h>

#define size(t) { t x; printf("%10s :\t%3zu bit\n", #t, CHAR_BIT * sizeof x); }

int main(int argc, char *argv[])
{
  size(char);
  size(short);
  size(int);
  size(long);
  size(double);
  size(void*);
  size(long long);

  return 0;
}
