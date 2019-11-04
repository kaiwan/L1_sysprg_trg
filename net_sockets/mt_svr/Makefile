# Makefile
# for the demo Internet domain C/S app...

CC=gcc
CFLAGS_DBG=-D_REENTRANT -DDBG -g -ggdb -O0 -Wall
CFLAGS=-D_REENTRANT -Wall

all: mt_sfingr

mt_sfingr: mt_sfingr.c mtfingr.h
	${CC} ${CFLAGS_DBG} mt_sfingr.c -o mt_sfingr -lpthread

cfingr: cfingr.c mtfingr.h
	${CC} ${CFLAGS_DBG} cfingr.c -o cfingr

clean:
	rm -f cfingr mt_sfingr
