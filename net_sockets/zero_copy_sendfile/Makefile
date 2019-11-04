# Makefile
# for the demo file transfer program; as read/write or zero-copy.

CC=gcc
CFLAGS_DBG=-D_REENTRANT -DDBG -g -ggdb -O0 -Wall
CFLAGS=-D_REENTRANT -Wall -Os

ALL := xfer_file
all: ${ALL}

xfer_file: xfer_file.c restart_lib.h
	${CC} ${CFLAGS} xfer_file.c -o xfer_file
xfer_file_dbg: xfer_file.c restart_lib.h
	${CC} ${CFLAGS_DBG} xfer_file.c -o xfer_file_dbg

clean:
	rm -f *.o ${ALL}

