# Makefile
#  for sample source for the training sessions.
#
# * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
# *
# *
# * This program is free software; you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation; either version 2 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program; if not, write to the Free Software
# * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
# 
CC := gcc
CFLAGS := -O2 -Wall
CFLAGS_DBG := -DDEBUG -O0 -g -ggdb -Wall
ALL := pipe1 pipe2 pipe-dup2 whopipe popen_tst

all: ${ALL}

pipe1: pipe1.c
	${CC} ${CFLAGS_DBG} pipe1.c -o pipe1

pipe2: pipe2.c
	${CC} ${CFLAGS_DBG} pipe2.c -o pipe2
pipe-dup2: pipe-dup2.c
	${CC} ${CFLAGS_DBG} pipe-dup2.c -o pipe-dup2

whopipe: whopipe.c
	${CC} ${CFLAGS_DBG} whopipe.c -o whopipe

popen_tst: popen_tst.c
	${CC} ${CFLAGS_DBG} popen_tst.c -o popen_tst
clean:
	rm -f *.o ${ALL}

