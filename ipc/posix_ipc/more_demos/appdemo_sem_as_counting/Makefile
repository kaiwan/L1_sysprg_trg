# Makefile
CFLAGS = -Wall -Os
CFLAGS_DBG = -Wall -Wextra -O0 -g
OUT := server_sem_as_counting client_sem_as_counting

all: ${OUT}

server_sem_as_counting: server_sem_as_counting.c
	gcc ${CFLAGS} server_sem_as_counting.c -o server_sem_as_counting -lrt
client_sem_as_counting: client_sem_as_counting.c
	gcc ${CFLAGS} client_sem_as_counting.c -o client_sem_as_counting -lrt

debug: client_sem_as_counting.c server_sem_as_counting.c
	gcc ${CFLAGS_DBG} server_sem_as_counting.c -o server_sem_as_counting_dbg -lrt
	gcc ${CFLAGS_DBG} client_sem_as_counting.c -o client_sem_as_counting_dbg -lrt

clean:
	rm -f ${OUT}
