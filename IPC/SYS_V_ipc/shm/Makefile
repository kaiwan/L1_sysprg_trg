# Makefile
# for sysvipc prgs

CFLAGS = -Wall -Os
CFLAGS_DBG = -Wall -O0 -g
OUT = shm1_r shm1_w

all: ${OUT}

shm1_w: shm1_w.c hdr_shm.h
	gcc ${CFLAGS} shm1_w.c -o shm1_w
shm1_w_dbg: shm1_w.c hdr_shm.h
	gcc ${CFLAGS_DBG} shm1_w.c -o shm1_w_dbg

shm1_r: shm1_r.c hdr_shm.h
	gcc ${CFLAGS} shm1_r.c -o shm1_r
shm1_r_dbg: shm1_r.c hdr_shm.h
	gcc ${CFLAGS_DBG} shm1_r.c -o shm1_r_dbg

clean:
	rm ${OUT}
