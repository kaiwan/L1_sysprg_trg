# Makefile
#  for sample source for the training sessions.
#  Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
ALL := pr_ids
all: ${ALL}
CC := gcc
CFLAGS_DBG := -g -ggdb -gdwarf-4 -O0 -Wall

pr_ids: pr_ids.c
	${CC} ${CFLAGS_DBG} pr_ids.c -o pr_ids_setuidroot
	sudo chown root:root pr_ids_setuidroot
	sudo chmod u+s pr_ids_setuidroot
clean:
	rm -fv pr_ids_setuidroot
