# Makefile
CFLAGS = -Wall -Os
CFLAGS_DBG = -Wall -Wextra -Og -g
OUT = reader writer reader_dbg writer_dbg

all: ${OUT}

reader: reader.c
	gcc ${CFLAGS} reader.c -o reader -lrt -pthread
writer: writer.c
	gcc ${CFLAGS} writer.c -o writer -lrt -pthread

reader_dbg: reader.c
	gcc ${CFLAGS_DBG} reader.c -o reader_dbg -lrt -pthread
writer_dbg: writer.c
	gcc ${CFLAGS_DBG} writer.c -o writer_dbg -lrt -pthread

clean:
	rm -f ${OUT} core*
