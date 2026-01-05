/*
 * Src: ChatGPT 4
 * A few addons by kaiwan
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_ITERATIONS 1000000

int balance = 0;  // Shared bank account balance

//----------------------------BAD CASE---------------------------------
void* deposit_bad(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        balance++;  // Read-modify-write without synchronization
    }
    return NULL;
}

void* withdraw_bad(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        balance--;  // Read-modify-write without synchronization
    }
    return NULL;
}
//---------------------------------------------------------------------

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* deposit(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        balance++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* withdraw(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        balance--;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}


int main(int argc, char **argv)
{
    pthread_t t1, t2;

    if (argc < 2) {
	fprintf(stderr, "Usage: %s opt\n\
opt=0 : bad case - classic data race as no mutex's used\n\
opt=1 : good case - NO data race as a mutex is used\n", argv[0]);
	exit(1);
    }

    if (atoi(argv[1]) == 0) {
	pthread_create(&t1, NULL, deposit_bad, NULL);
	pthread_create(&t2, NULL, withdraw_bad, NULL);
    } else if (atoi(argv[1]) == 1) {
	pthread_create(&t1, NULL, deposit, NULL);
	pthread_create(&t2, NULL, withdraw, NULL);
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final balance: %d\n", balance);
    return 0;
}

