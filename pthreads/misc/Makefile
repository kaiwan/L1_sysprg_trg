# Makefile for show dir pthread programs..

CFLAGS = -Wall
CFLAGS_DBG = -Wall -g -ggdb -gdwarf-4 -O0 -DDEBUG -Wunused-but-set-variable
LINKIN = -lpthread

# UPDATE CXX for your system!
CXX := /CodeSourcery_toolchain/arm-none-linux-gnueabi

OUT = tslp thrd4 tsig main_dies vmuser_pthrds attr_setget rlim_threads cr8ManyThrds main_dies worker_dies
CB_FILES = tslp.c thrd4.c tsig.c main_dies.c vmuser_pthrds.c rlim_threads.c attr_setget.c cr8ManyThrds.c main_dies.c worker_dies.c

all: ${OUT}

main_dies: main_dies.c
	gcc ${CFLAGS} main_dies.c -o main_dies ${LINKIN}
worker_dies: worker_dies.c
	gcc ${CFLAGS} worker_dies.c -o worker_dies ${LINKIN}

cr8ManyThrds: cr8ManyThrds.c
	gcc ${CFLAGS} cr8ManyThrds.c -o cr8ManyThrds ${LINKIN}

tslp: tslp.c libpk.h
	gcc ${CFLAGS} tslp.c -o tslp ${LINKIN}
tslp-debug: tslp.c libpk.h
	gcc ${CFLAGS_DBG} -g tslp.c -o tslp-debug ${LINKIN}

vmuser_pthrds: vmuser_pthrds.c
	gcc ${CFLAGS} vmuser_pthrds.c -o vmuser_pthrds ${LINKIN}

rlim_threads: rlim_threads.c
	gcc ${CFLAGS_DBG} rlim_threads.c -o rlim_threads ${LINKIN}

attr_setget: attr_setget.c
	gcc ${CFLAGS} attr_setget.c -o attr_setget ${LINKIN}

thrd4: thrd4.c
	gcc ${CFLAGS} thrd4.c -o thrd4 ${LINKIN}

tsig: tsig.c libpk.h
	gcc ${CFLAGS} tsig.c -o tsig ${LINKIN}
tsig_dbg: tsig.c libpk.h
	gcc ${CFLAGS_DBG} tsig.c -o tsig_dbg ${LINKIN}

clean:
	rm -f ${OUT} *.o
	touch *.c *.h

# CB - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note- original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}
