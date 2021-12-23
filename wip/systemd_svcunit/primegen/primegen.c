#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MAX    10000000		// 10 million

static void simple_primegen(int limit)
{
	int i, j, num = 2, isprime;

	printf("  2,  3, ");
	for (i = 4; i <= limit; i++) {
		isprime = 1;
		for (j = 2; j < limit / 2; j++) {
			if ((i != j) && (i % j == 0)) {
				isprime = 0;
				break;
			}
		}
		if (isprime) {
			num++;
			printf("%6d, ", i);
			/* Wrap after WRAP primes are printed on a line;
			 * this is crude; in production code, one must query
			 * the terminal window's width and calculate the column
			 * to wrap at.
			 */
#define WRAP    16
			if (num % WRAP == 0)
				printf("\n");
		}
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	int limit;

	if (argc < 2) {
		fprintf(stderr,
			"Usage: %s limit-to-generate-primes-upto\n"
			" max is %d\n", argv[0], MAX);
		exit(EXIT_FAILURE);
	}

	limit = atoi(argv[1]);
	if (limit < 4 || limit > MAX) {
		fprintf(stderr,
			"%s: invalid value (%d); pl pass a value within "
			"the range [4 - %d].\n", argv[0], limit, MAX);
		exit(EXIT_FAILURE);
	}
	simple_primegen(limit);
	exit(EXIT_SUCCESS);
}
