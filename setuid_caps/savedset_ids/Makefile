# Makefile
#  for sample source for the training sessions.
#  Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
ALL := savedset_ids
all: ${ALL}
CC := gcc
CFLAGS_DBG := -g -ggdb -gdwarf-4 -O0 -Wall

savedset_ids: savedset_ids.c
	${CC} ${CFLAGS_DBG} savedset_ids.c -o savedset_ids
	sudo chown root:root savedset_ids
	sudo chmod u+s savedset_ids
clean:
	rm -fv ${ALL}
