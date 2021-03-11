# Makefile
ALL := fork_test pthread_test
CC := ${CROSS_COMPILE}gcc

all: ${ALL}
fork_test: fork_test.c
	${CC} fork_test.c -o fork_test -Wall -O2
pthread_test: pthread_test.c
	${CC} pthread_test.c -o pthread_test -Wall -pthread -O2
clean:
	rm -fv ${ALL}
