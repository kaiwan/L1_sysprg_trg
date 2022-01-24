# Makefile
# for the demo Internet domain C/S app...

CC=gcc
CFLAGS_DBG=-DDBG -g -ggdb -O0 -Wall
CFLAGS=-Wall -O2 -UDBG

ALL := client_any
all: ${ALL}
CB_FILES := client_any.c

client_any: client_any.c
	${CC} ${CFLAGS_DBG} client_any.c -o client_any

# CB - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note- original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 ${CB_FILES}

clean:
	rm -f ${ALL}
