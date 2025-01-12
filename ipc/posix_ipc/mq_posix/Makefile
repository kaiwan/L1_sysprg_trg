# Makefile
# for sysvipc prgs

# -m32 : compile for 32-bit

CFLAGS = -Wall -Os
CFLAGS_DBG = -Wall -O0 -g
OUT = sender_mq recv_mq

all: ${OUT}

sender_mq: sender_mq.c
	gcc ${CFLAGS} sender_mq.c -o sender_mq -lrt
sender_mq_dbg: sender_mq.c
	gcc ${CFLAGS_DBG} sender_mq.c -o sender_mq_dbg -lrt

recv_mq: recv_mq.c
	gcc ${CFLAGS} recv_mq.c -o recv_mq -lrt
sender_mq_dbg_dbg: recv_mq.c
	gcc ${CFLAGS_DBG} recv_mq.c -o recv_mq_dbg -lrt

clean:
	rm -f ${OUT} *_dbg

