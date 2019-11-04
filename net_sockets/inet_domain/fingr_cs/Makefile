# Makefile
# for the demo Internet domain C/S app...

CC=gcc
CFLAGS_DBG=-D_REENTRANT -DDBG -g -ggdb -O0 -Wall
CFLAGS=-D_REENTRANT -Wall

all: cfingr sfingr

sfingr: sfingr.c fingr.h
	${CC} ${CFLAGS_DBG} sfingr.c -o sfingr

cfingr: cfingr.c fingr.h
	${CC} ${CFLAGS_DBG} cfingr.c -o cfingr

clean:
	rm -f cfingr sfingr
