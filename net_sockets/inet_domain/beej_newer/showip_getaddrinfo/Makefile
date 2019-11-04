# Makefile

CC=gcc
CFLAGS_DBG=-D_REENTRANT -DDBG -g -ggdb -O0 -Wall
CFLAGS=-D_REENTRANT -Wall

ALL := showip
all: ${ALL}
CB_FILES := showip.c

showip: showip.c
	${CC} ${CFLAGS_DBG} showip.c -o showip

# CB - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note- original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}
#	indent -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 ${CB_FILES}

clean:
	rm -f ${ALL}
