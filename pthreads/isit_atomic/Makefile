# Makefile

ALL := isit_atomic mips_isit_atomic arm_isit_atomic

ARM_CXX=arm-none-linux-gnueabi-
MIPS_CXX=mips-linux-gnu-
CC=gcc
OBJDUMP=objdump
CFLAGS_DBG=-g -ggdb -gdwarf-4 -O0 -Wall
CFLAGS=-Wall

all: ${ALL}
CB_FILES := *.[ch]

isit_atomic: isit_atomic.c
	${CC} ${CFLAGS_DBG} isit_atomic.c -o isit_atomic
	objdump -dS -M intel ./isit_atomic > isit_atomic.disas.x86
mips_isit_atomic: isit_atomic.c
	${MIPS_CXX}${CC} ${CFLAGS_DBG} isit_atomic.c -o mips_isit_atomic
	${MIPS_CXX}${OBJDUMP} -dS ./mips_isit_atomic > isit_atomic.disas.mips
arm_isit_atomic: isit_atomic.c
	${ARM_CXX}${CC} ${CFLAGS_DBG} isit_atomic.c -o arm_isit_atomic
	${ARM_CXX}${OBJDUMP} -dS ./arm_isit_atomic > isit_atomic.disas.arm

# cb - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note! original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}

clean:
	rm -vf ${ALL} *.o
