# Makefile
#  for sample source for the training sessions.
#
# * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
# * MIT License.

ALL := execl1 envp exec_poc
all: ${ALL}
CC := gcc
CFLAGS := -O2 -Wall
CFLAGS_DBG := -DDEBUG -O0 -g -ggdb -Wall

execl1: execl1.c
	${CC} ${CFLAGS_DBG} execl1.c -o execl1

envp: envp.c
	${CC} ${CFLAGS_DBG} envp.c -o envp

exec_poc: exec_poc.c
	${CC} ${CFLAGS_DBG} exec_poc.c -o exec_poc

clean:
	rm -f *.o ${ALL} 2>/dev/null
