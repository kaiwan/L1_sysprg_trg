# Makefile
#  for sample source for the training sessions.
#
# * Author: Kaiwan N Billimoria <kaiwan@designergraphix.com>
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

all: prg12.9 whichexit

whichexit: whichexit.c
	${CC} ${CFLAGS_DBG} whichexit.c -o whichexit -lpthread

prg12.9: prg12.9.c
	${CC} ${CFLAGS_DBG} prg12.9.c -o prg12.9 -lpthread

clean:
	rm -f *.o ${all}

