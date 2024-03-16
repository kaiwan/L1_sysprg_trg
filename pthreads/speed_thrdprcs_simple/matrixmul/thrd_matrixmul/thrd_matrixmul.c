// parallel program to multiply two nxn matrices- matrix_mul_pthreads.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define n 2048 //1024  //5120			// global space
#define nthreads 12              // 'n' => # cores on the box
int a[n][n], b[n][n], c[n][n];

void *threadfun(void *arg)	// Each Thread Will do this.
{
	int *p = (int *)arg;
	int i, j, k;

	for (i = *p; i < (*p + (n / nthreads)); i++)
		for (j = 0; j < n; j++)
			for (k = 0; k < n; k++)
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
	pthread_exit((void *)0);
}

int main()
{
	int i, j, rownos[nthreads];
	pthread_t tid[nthreads];

	for (i = 0; i < n; i++) {	// Data Initialization.
		for (j = 0; j < n; j++) {
			a[i][j] = 1;
			b[i][j] = 1;
		}
	}

// thread creations using pthreads API.
	for (i = 0; i < nthreads; i++) {
		rownos[i] = i * (n / nthreads);
		pthread_create(&tid[i], NULL, threadfun, &rownos[i]);
	}

// making main thread to wait until all other threads complete using pthreads API.
	for (i = 0; i < nthreads; i++)
		pthread_join(tid[i], NULL);

#if 0
 printf("The result of multiplication is:\n:");
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			printf("%d ", c[i][j]);

		printf("\n");
	}
#endif
	pthread_exit((void *)0);
}
