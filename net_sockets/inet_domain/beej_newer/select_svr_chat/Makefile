# Makefile
# for the demo Internet domain C/S app...

CC=gcc
CFLAGS_DBG=-D_REENTRANT -DDBG -g -ggdb -O0 -Wall
CFLAGS=-D_REENTRANT -Wall

ALL := selectserver
all: ${ALL}
CB_FILES := selectserver.c

selectserver: selectserver.c
	${CC} ${CFLAGS_DBG} selectserver.c -o selectserver

# CB - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note- original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}

clean:
	rm -f ${ALL}
