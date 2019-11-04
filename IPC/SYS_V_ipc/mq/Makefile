# Makefile
# for sysvipc prgs

# -m32 : compile for 32-bit

CFLAGS = -Wall -Os #-m32
CFLAGS_DBG = -Wall -O0 -g #-m32
OUT = msg1_r msg1_w

all: ${OUT}

msg1_r: msg1_r.c
	gcc ${CFLAGS} msg1_r.c -o msg1_r
msg1_r_dbg: msg1_r.c
	gcc ${CFLAGS_DBG} msg1_r.c -o msg1_r_dbg

msg1_w: msg1_w.c
	gcc ${CFLAGS} msg1_w.c -o msg1_w
msg1_w_dbg: msg1_w.c
	gcc ${CFLAGS_DBG} msg1_w.c -o msg1_w_dbg

clean:
	rm ${OUT} msg1_r_dbg msg1_w_dbg
