# Makefile for show dir pthread programs..

CFLAGS = -Wall -g -ggdb -O0
LINKIN = -pthread
OUT = mt_fork_exec

all: ${OUT}

mt_fork_exec: mt_fork_exec.c
	gcc ${CFLAGS} mt_fork_exec.c -o mt_fork_exec ${LINKIN}

clean:
	rm -fv ${OUT}
