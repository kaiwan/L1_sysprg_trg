# Makefile
# for sysvipc prgs

CFLAGS = -Wall -Os
CFLAGS_DBG = -Wall -O0 -g
OUT = semdemo semrm

all: ${OUT}

semdemo: semdemo.c
	gcc ${CFLAGS} semdemo.c -o semdemo
semrm: semrm.c
	gcc ${CFLAGS} semrm.c -o semrm
clean:
	rm ${OUT} *.o

