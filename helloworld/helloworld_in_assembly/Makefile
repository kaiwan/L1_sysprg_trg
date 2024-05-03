# Makefile
# For x86_64 assembly
ALL := hello

CC=gcc
LD=ld
CFLAGS_DBG=-g -ggdb -O0 -Wall
CFLAGS=-c -Wall -O2

all: ${ALL}

hello: hello.o
	${LD} -o $@ $+
hello.o : hello.s
	${CC} ${CFLAGS} -o $@ $<

clean:
	rm -fv ${ALL} *.o
