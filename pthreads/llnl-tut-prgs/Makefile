# Makefile for show dir pthread programs..

CFLAGS = -Wall
CFLAGS_DBG = -Wall -g -ggdb -gdwarf-4 -O0 -DDEBUG
LINKIN = -pthread

# UPDATE CXX for your system!
CXX := /CodeSourcery_toolchain/arm-none-linux-gnueabi

OUT = pt1 fork_test pthread_test pt2-join pt3-matrixmul stacksz matrixmul-dbg
CB_FILES := pt1.c fork_test.c pthread_test.c pt2-join.c pt3-matrixmul.c stacksz.c matrixmul-dbg.c
all: ${OUT}

pt1: pt1.c
	gcc ${CFLAGS} pt1.c -o pt1 ${LINKIN}

fork_test: fork_test.c
	gcc ${CFLAGS} fork_test.c -o fork_test ${LINKIN}
pthread_test: pthread_test.c
	gcc ${CFLAGS} pthread_test.c -o pthread_test ${LINKIN}

pt2-join: pt2-join.c
	gcc ${CFLAGS} pt2-join.c -o pt2-join ${LINKIN}

matrixmul-dbg: matrixmul-dbg.c convenient.h
	gcc ${CFLAGS} -DDEBUG matrixmul-dbg.c -o matrixmul-dbg ${LINKIN}

pt3-matrixmul: pt3-matrixmul.c
	gcc ${CFLAGS} pt3-matrixmul.c -o pt3-matrixmul ${LINKIN}

stacksz: stacksz.c
	gcc ${CFLAGS_DBG} stacksz.c -o stacksz ${LINKIN}

cb: ${CB_FILES}
	mkdir bkp 2>/dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}

clean:
	rm -f ${OUT} *.o
	touch *.c *.h
