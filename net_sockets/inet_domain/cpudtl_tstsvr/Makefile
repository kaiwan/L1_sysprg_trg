# Makefile
# for the demo Internet domain C/S app...

ALL := cpudtl_tstsvr
all: ${ALL}
CB_FILES := cpudtl_tstsvr.c

CC=gcc
CFLAGS_DBG=-D_REENTRANT -DDEBUG -DDBG -g -ggdb -O0 -Wall
CFLAGS=-D_REENTRANT -Wall

cpudtl_tstsvr: cpudtl_tstsvr.c
	${CC} ${CFLAGS_DBG} cpudtl_tstsvr.c -o cpudtl_tstsvr

# CB - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note- original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}

clean:
	rm -f ${ALL}
