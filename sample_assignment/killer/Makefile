# Makefile
# For 'Linux System Programming' training session from Kaiwan (kaiwanTECH)

## Pl check and keep or remove <foo>_dbg_[asan|ub|msan] targets
## as desired.
FNAME_C := killer
ALL :=  ${FNAME_C} ${FNAME_C}_dbg ${FNAME_C}_dbg_asan ${FNAME_C}_dbg_ub ${FNAME_C}_dbg_msan

GCC=${CROSS_COMPILE}gcc
CLANG=${CROSS_COMPILE}clang

# Try using 'clang' by default; if it's not installed, fallback to using 'gcc'
CXX=${CLANG}
CLANG_CHK=$(shell which clang)
ifeq (${CLANG_CHK},)
  CXX=${GCC}
endif
$(info === Compiler : $(shell $(CXX) --version|head -n1) )

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
${FNAME_C}: ${FNAME_C}.c
	${CXX} ${CFLAGS} ${FNAME_C}.c -o ${FNAME_C}
${FNAME_C}_dbg: ${FNAME_C}.c
	${CXX} ${CFLAGS_DBG} ${FNAME_C}.c -o ${FNAME_C}_dbg

#--- Sanitizers (use clang): <foo>_dbg_[asan|ub|msan]
${FNAME_C}_dbg_asan: ${FNAME_C}.c
	${CXX} ${CFLAGS_DBG_ASAN} ${FNAME_C}.c -o ${FNAME_C}_dbg_asan
${FNAME_C}_dbg_ub: ${FNAME_C}.c
	${CXX} ${CFLAGS_DBG_UB} ${FNAME_C}.c -o ${FNAME_C}_dbg_ub
${FNAME_C}_dbg_msan: ${FNAME_C}.c
	${CXX} ${CFLAGS_DBG_MSAN} ${FNAME_C}.c -o ${FNAME_C}_dbg_msan

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
	${CXX} ${CFLAGS_GCOV} ${FNAME_C}.c -o ${FNAME_C}_gcov
	./${FNAME_C}_gcov
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

help:
	@echo '=== Makefile Help : additional targets available ==='
	@echo 'checkpatch : run the kernel code style checker tool on source file(s)'
	@echo 'clean      : cleanup - remove all the binaries, core files, etc'
	@echo 'covg       : run the gcov code coverage tool on the source'
	@echo 'da         : run the dynamic analysis tool ($(VALGRIND)) on the binary executable'
	@echo 'help       : this 'help' target'
	@echo 'indent     : run the $(INDENT) utility on source file(s) to indent them as per the kernel code style'
	@echo 'package    : tar and compress the source files into the dir above'
	@echo 'sa         : run the static analysis tool ($(FLAWFINDER)) on the source file(s)'
