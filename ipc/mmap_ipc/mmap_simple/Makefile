# Makefile
ALL := mmap_simple mmap_simple_dbg

all: ${ALL}
CB_FILES := *.[ch]

mmap_simple: mmap_simple.c convenient.h
	gcc mmap_simple.c -o mmap_simple -Os -Wall #-DDEBUG
mmap_simple_dbg: mmap_simple.c convenient.h
	gcc mmap_simple.c -o mmap_simple_dbg -O0 -g -ggdb -Wall -DDEBUG

# CB - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note- original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}
clean:
	rm -f ${ALL}

