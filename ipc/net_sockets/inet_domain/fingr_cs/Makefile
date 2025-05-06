# Makefile
# for the demo Internet domain C/S app...

CC=gcc
CFLAGS_DBG=-DDBG -g -ggdb -Og -Wall -Wextra
CFLAGS=-O2 -Wall -Wextra

ALL := cfingr sfingr cfingr_dbg sfingr_dbg
all: ${ALL}

sfingr: sfingr.c fingr.h
	${CC} ${CFLAGS} sfingr.c -o sfingr
cfingr: cfingr.c fingr.h
	${CC} ${CFLAGS} cfingr.c -o cfingr

sfingr_dbg: sfingr.c fingr.h
	${CC} ${CFLAGS_DBG} sfingr.c -o sfingr_dbg
cfingr_dbg: cfingr.c fingr.h
	${CC} ${CFLAGS_DBG} cfingr.c -o cfingr_dbg

clean:
	rm -f ${ALL}
