# Makefile
# for sysvipc prgs

CFLAGS = -Wall -Os
CFLAGS_DBG = -Wall -O0 -g
OUT = sem1

all: ${OUT}

sem1: sem1.c
	gcc ${CFLAGS} sem1.c -o sem1
sem1_debug: sem1.c
	gcc ${CFLAGS_DBG} sem1.c -o sem1_dbg

clean:
	rm ${OUT}
