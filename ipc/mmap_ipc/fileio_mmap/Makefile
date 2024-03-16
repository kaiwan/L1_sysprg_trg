# Makefile
ALL := mmap_fileio mmap_fileio_dbg

all: ${ALL}
CB_FILES := *.[ch]

mmap_fileio: mmap_fileio.c
	gcc mmap_fileio.c -o mmap_fileio -Wall -O3 #-DDEBUG
mmap_fileio_dbg: mmap_fileio.c
	gcc -g -ggdb -gdwarf-4 -fno-omit-frame-pointer mmap_fileio.c -o mmap_fileio_dbg -Wall -O0 #-DDEBUG

# CB - C Beautifier ! uses indent- "beautifies" C code into the "Linux kernel style"
# Note- original source file(s) is overwritten
cb: ${CB_FILES}
	mkdir bkp 2> /dev/null; cp -f ${CB_FILES} bkp/
	indent -linux ${CB_FILES}
clean:
	rm -fv ${ALL}

