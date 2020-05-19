# Makefile
# For 'Linux System Programming' training session from Kaiwan (kaiwanTECH)
# held on ______________
# Organization: ____________

## Pl check and keep or remove <foo>_dbg_[asan|ub|msan] targets
## as desired.
ALL :=  killer killer_dbg killer_dbg_asan killer_dbg_ub killer_dbg_msan

CC=${CROSS_COMPILE}gcc
CL=${CROSS_COMPILE}clang

CFLAGS=-Wall -UDEBUG
# Dynamic analysis includes the compiler itself!
# Especially the powerful Address Sanitizer toolset
CFLAGS_DBG=-g -ggdb -gdwarf-4 -O0 -Wall -Wextra -DDEBUG
CFLAGS_DBG_ASAN=${CFLAGS_DBG} -fsanitize=address
CFLAGS_DBG_MSAN=${CFLAGS_DBG} -fsanitize=memory
CFLAGS_DBG_UB=${CFLAGS_DBG} -fsanitize=undefined

CFLAGS_GCOV=${CFLAGS_DBG} -fprofile-arcs -ftest-coverage

# Required vars
all: ${ALL}
SRC_FILES := *.[ch]
INDENT := indent
CHECKPATCH := ../../checkpatch.pl
FLAWFINDER := flawfinder
VALGRIND := valgrind
PKG_NAME := killer

# Targets and their rules
killer: killer.c
	${CC} ${CFLAGS} killer.c -o killer
killer_dbg: killer.c
	${CC} ${CFLAGS_DBG} killer.c -o killer_dbg

#--- Sanitizers (use clang): <foo>_dbg_[asan|ub|msan]
killer_dbg_asan: killer.c
	${CL} ${CFLAGS_DBG_ASAN} killer.c -o killer_dbg_asan
killer_dbg_ub: killer.c
	${CL} ${CFLAGS_DBG_UB} killer.c -o killer_dbg_ub
killer_dbg_msan: killer.c
	${CL} ${CFLAGS_DBG_MSAN} killer.c -o killer_dbg_msan

# indent- "beautifies" C code - to conform to the the Linux kernel
# coding style guidelines.
# Note! original source file(s) is overwritten, so we back it up.
indent: ${SRC_FILES}
	mkdir bkp 2> /dev/null; cp -f ${SRC_FILES} bkp/
	${INDENT} -linux ${SRC_FILES}

checkpatch:
	${CHECKPATCH} -f --no-tree ${SRC_FILES}

sa:   # static analysis
	${FLAWFINDER} --neverignore --context ${SRC_FILES}

da:   # dynamic analysis
	${VALGRIND} --tool=memcheck --trace-children=yes ./killer_dbg

# Testing: line coverage with gcov(1)
covg:
	${CC} ${CFLAGS_GCOV} killer.c -o killer_gcov
	./killer_gcov
	gcov ${SRC_FILES}
	$(info === Code coverage: examine the *.c.gcov file(s) ===)

# packaging
package:
	rm -f ../${PKG_NAME}.tar.xz
	make clean
	tar caf ../${PKG_NAME}.tar.xz *
	ls -l ../${PKG_NAME}.tar.xz
	$(info === $(PKG_NAME).tar.xz package created ===)

clean:
	rm -vf ${ALL} core* vgcore* *.o *~ *.c.gcov *.gcda *.gcno
