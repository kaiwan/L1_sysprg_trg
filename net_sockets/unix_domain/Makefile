# Makefile
# for the demo Unix domain C/S app...

ALL := unix_svr unix_cli

CC=gcc
CFLAGS_DBG=-D_REENTRANT -g -ggdb -O0 -Wall
CFLAGS=-D_REENTRANT -Wall

all: ${ALL}
CB_FILES := *.[ch]

unix_svr: unix_svr.c unet.h
	${CC} ${CFLAGS} unix_svr.c -o unix_svr

unix_cli: unix_cli.c unet.h
	${CC} ${CFLAGS} unix_cli.c -o unix_cli

# CB - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note- original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}

clean:
	rm -f ${ALL}

