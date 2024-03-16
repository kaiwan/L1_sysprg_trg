// serial program to multiply two nxn matrices -matrix_mul_serial.c
#include <stdio.h>
#include <stdlib.h>
// global space
#define n 2048 //1024 //5120
int a[n][n], b[n][n], c[n][n];

int main()
{
	int i, j, k;
	for (i = 0; i < n; i++)	// Data Initialization
		for (j = 0; j < n; j++) {
			a[i][j] = 1;
			b[i][j] = 1;
		}
	for (i = 0; i < n; i++)	// Multiplication
		for (j = 0; j < n; j++)
			for (k = 0; k < n; k++)
				c[i][j] = c[i][j] + a[i][k] * b[k][j];

#if 0
	printf("The resultant matrix is \n:");

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			printf("%d ", c[i][j]);
		printf("\n");
	}
#endif
}
