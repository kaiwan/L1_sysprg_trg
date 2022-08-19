# Makefile
#  for sample source for the training sessions.
CC := gcc
CFLAGS := -O2 -Wall
CFLAGS_DBG := -DDEBUG -O0 -g -ggdb -Wall
ALL := wr_fifo rd_fifo

all: ${ALL}

wr_fifo: wr_fifo.c
	${CC} ${CFLAGS_DBG} wr_fifo.c -o wr_fifo
rd_fifo: rd_fifo.c
	${CC} ${CFLAGS_DBG} rd_fifo.c -o rd_fifo
clean:
	rm -f *.o ${ALL}

