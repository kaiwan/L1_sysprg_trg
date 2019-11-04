# Makefile
# for sysvipc prgs

CFLAGS = -Wall -Os
CFLAGS_DBG = -Wall -O0 -g
OUT = sem1

all: ${OUT}

sem1.o: sem1.c sem_lib.h
	gcc ${CFLAGS} -c sem1.c -o sem1.o
sem1: sem1.o
	gcc ${CFLAGS} -o sem1 sem1.o
sem1_debug: sem1.c sem_lib.h
	gcc ${CFLAGS_DBG} sem1.c -o sem1_dbg

sem2: sem2.c sem_lib.h
	gcc ${CFLAGS} sem2.c -o sem2

clean:
	rm -fv *.o sem[12] sem[12]_dbg
