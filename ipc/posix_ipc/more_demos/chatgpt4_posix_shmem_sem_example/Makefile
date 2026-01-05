# Makefile
# A 'better' Makefile template for Linux system programming.
#
# Designed to support building C projects with MULTIPLE source files.
# Handles separate compilation and linking of object files.
#
# Besides the 'usual' targets to build production and debug versions of the
# code and cleanup, we incorporate targets to do useful (and indeed required)
# stuff like:
#  - prod_2part: build a '2-part' production target 'prod_2part'; it's
#     -O${PROD_OPTLEVEL}, no debug symbolic info, strip-debug;
#     Excellent for production as it gives ability to debug as and when required!
#  - indent: adhering to (Linux kernel) coding style guidelines (indent+checkpatch)
#  - sa: static analysis target (via flawfinder, cppcheck)
#  - dynamic analysis target: via valgrind
#  - code coverage via gcov/lcov
#  - a packaging (.tar.xz) target and
#  - a help target.
#
# You will require these utils installed:
#  indent, flawfinder, cppcheck, valgrind, kernel-headers package -or- simply the
#  checkpatch.pl script, gcov, lcov, tar; + libasan
#  The lcov_gen.sh wrapper script
#   (DEV NOTE: careful of dependencies between this Makefile and script)
#
# To get started, just type:
#  make help
#
# (c) 2020 Kaiwan N Billimoria, kaiwanTECH
# License: MIT

## Pl check and keep or remove <foo>_dbg_[asan|ub|msan] targets
## (where <foo> is the program name) as desired.
ALL :=  prod prod_2part

###
# Update as required
# Set the program names (executable names) here:
PROG_READER := reader
PROG_WRITER := writer

# List all C source files for each program
# ONLY list .c files, not headers
SRCS_READER := reader.c
SRCS_WRITER := writer.c

# Automatically generate object file names from source files
OBJS_READER := $(SRCS_READER:.c=.o)
OBJS_WRITER := $(SRCS_WRITER:.c=.o)

# For coverage builds, we need alternate object file names
OBJS_READER_GCOV := $(SRCS_READER:.c=.gcov.o)
OBJS_WRITER_GCOV := $(SRCS_WRITER:.c=.gcov.o)

# Convenience variable for all object files
OBJS := $(OBJS_READER) $(OBJS_WRITER)

#--- CHECK: manually add params as required
# Populate any required cmdline arguments to the process here:
CMDLINE_ARGS=""


# Can remove the 'msan' one if it doesn't build - requires clang
# Build targets for each program
ALL_NM := ${PROG_READER} ${PROG_WRITER} \
	${PROG_READER}_dbg ${PROG_WRITER}_dbg \
	${PROG_READER}_dbg_asan ${PROG_WRITER}_dbg_asan \
	${PROG_READER}_dbg_ub ${PROG_WRITER}_dbg_ub \
	${PROG_READER}_dbg_lsan ${PROG_WRITER}_dbg_lsan \
	${PROG_READER}_gcov ${PROG_WRITER}_gcov \
	${PROG_READER}_dbg_msan ${PROG_WRITER}_dbg_msan \
	${PROG_READER}_dbg_tsan ${PROG_WRITER}_dbg_tsan

# Decide which compiler to use; GCC doesn't support MSAN, clang does
CC := ${CROSS_COMPILE}gcc
LINKIN := -static-libasan   # use this lib for ASAN with GCC
ifeq (, $(shell which clang))
  @echo -n "\e[1m\e[41m"
  $(warning === WARNING! No clang (compiler) in PATH (reqd for MSAN); consider doing 'sudo apt install clang' or equivalent ===)
  @echo -n "\e[0m"
else
  CC := clang
  LINKIN := -static-libsan
endif
# Below lines for when you want to force GCC as the compiler
CC := ${CROSS_COMPILE}gcc
LINKIN := -static-libasan 
$(info Compiler set to $(CC); LINKIN = $(LINKIN))

STRIP=${CROSS_COMPILE}strip
STRIP_SPEC=--strip-unneeded
# "Remove all symbols that are not needed for relocation processing in addition to debugging symbols and sections stripped by --strip-debug"
READELF=${CROSS_COMPILE}readelf
OBJCOPY=${CROSS_COMPILE}objcopy

CSTD=-ansi -std=c99 -std=c11 -std=c18  # the last one wins; else if unsupported, earlier ones...
# For the meaning of the following feature test macros, see feature_test_macros(7)
POSIX_STD=201112L
STD_DEFS=-D_DEFAULT_SOURCE -D_GNU_SOURCE

# Add this option switch to CFLAGS / CFLAGS_DBG if you want ltrace to work on Ubuntu!
# (Although ltrace now works fine on recent versions of Ubuntu)
LTRACE_ENABLE=-z lazy

PROD_OPTLEVEL=3
  # -O<N>; N can be 0-3 or s (optimize for speed & size);
  #  in the -D_FORTIFY_SOURCE=N option, you must set N to the same number
DEBUG_OPTLEVEL=0  # g
  # -O<N>; N can be 0,1,2,3 or s (s: optimize for speed & size);
  # NOTE: in the -D_FORTIFY_SOURCE=N option, you must set N to the same number

#--- Security-related compiler flags !
# Ref: GCC security related flags reference [https://gist.github.com/kaiwan/d82d8271459c90c3accc3f795dcaf228]
SECURITY_CFLAGS_REG = -fstack-protector-strong \
	-O${PROD_OPTLEVEL} -D_FORTIFY_SOURCE=${PROD_OPTLEVEL} \
	-pie -fPIE \
	-fsanitize=bounds -fsanitize-undefined-trap-on-error \
	-fcf-protection \
	-fsanitize=signed-integer-overflow -fsanitize-undefined-trap-on-error
### NOTE- the -fcf-protection* flag(s) might not work on all archs ###

SECURITY_CFLAGS = ${SECURITY_CFLAGS_REG}
ifeq (gcc, $(CC))
   SECURITY_CFLAGS = ${SECURITY_CFLAGS_REG} -fstrict-flex-arrays
endif

# Compiler warnings
WARN = -Wall -Wextra -Wstack-protector -Wformat -Werror=format-security
WARN_MORE = ${WARN} -Wpedantic #-Werror

CFLAGS = -UDEBUG ${WARN} ${CSTD} -D_POSIX_C_SOURCE=${POSIX_STD} ${STD_DEFS} \
	${SECURITY_CFLAGS}
CFLAGS_DBG = -DDEBUG -g -ggdb -gdwarf-4 -O$(DEBUG_OPTLEVEL) \
	$(WARN) ${WARN_MORE} -fno-omit-frame-pointer \
	${CSTD} -D_POSIX_C_SOURCE=${POSIX_STD} ${STD_DEFS} \
	${SECURITY_CFLAGS}
# Optimization level for debug binaries: -Og vs -O0
# man gcc
# '... Optimize debugging experience.  -Og should be the optimization level of
# choice for the standard edit-compile-debug cycle, offering a reasonable level
# of  optimization  while maintaining  fast  compilation  and a good debugging
# experience. It is a better choice than -O0 for producing debuggable code
# because some compiler passes that collect debug information are disabled at -O0.'
#
# Having said that, I find that - esp for smaller programs - using -Og can cause
# the compiler to not really generate the code as required! Thus we can then
# have side effects, including GDB failing to find breakpoints and other weird
# errors...
# Hence, we keep the debug optimization level flag as -O0 ; feel free to change
# it - via the variable DEBUG_OPTLEVEL above - to 'g' if applicable.
# So botton line: for larger projects, we recommend -Og, smaller code -O0 .

# For MSan, don't use the -D_FORTIFY_SOURCE option
CFLAGS_DBG_ASAN=${CFLAGS_DBG} -fsanitize=address -fsanitize-address-use-after-scope
CFLAGS_DBG_UB=${CFLAGS_DBG} -fsanitize=undefined
CFLAGS_DBG_MSAN=${CFLAGS_DBG} -fsanitize=memory -fPIE -pie
CFLAGS_DBG_LSAN=${CFLAGS_DBG} -fsanitize=leak
CFLAGS_DBG_TSAN=${CFLAGS_DBG} -fsanitize=thread

CFLAGS_GCOV=${CFLAGS_DBG} -fprofile-arcs -ftest-coverage -lgcov


#--- Linker flags
# -Wl,-z,now -Wl,-z,relro : enable 'full RELRO' (relocation read-only)
LDFLAGS1=-Wl,-z,now -Wl,-z,relro
# -Wl,-z,noexecstack : enable non-executable stack
LDFLAGS2=-Wl,-z,noexecstack  #, -Wl,-z,noexecheap
LDFLAGS = ${LDFLAGS1} ${LDFLAGS2}
# For Windows add:
# -Wl,dynamicbase : tell linker to use ASLR protection
# -Wl,nxcompat : tell linker to use DEP protection
$(info LDFLAGS = $(LDFLAGS))

# glibc >= 2.34, libpthread is a part of it; hence were not required to link
# with -pthread any longer
#LINK=  #-pthread

# Required vars
all: ${ALL}
SRC_FILES := *.[ch]
INDENT := indent
CLANGTIDY := clang-tidy
FLAWFINDER := flawfinder
CPPCHECK := cppcheck
VALGRIND := valgrind
# update as required
PKG_NAME := shmem_sem_demo
CHECKPATCH := /lib/modules/$(shell uname -r)/build/scripts/checkpatch.pl
GCOV := ${CROSS_COMPILE}gcov
LCOV := lcov
GENINFO := geninfo
GENHTML := genhtml
LCOV_SCRIPT := lcov_gen.sh
CHECKSEC := checksec

# Targets and their rules
# Three types:
# 1. 'regular' production target 'prod': -O${PROD_OPTLEVEL}, no debug symbolic info, stripped
# 2. '2-part' production target 'prod_2part': -O${PROD_OPTLEVEL}, no debug symbolic info, strip-debug;
#     excellent for production as it gives ability to debug as and when required!
#     (internally invokes the 'debug' target as it requires the debug binary as well
# 3. 'debug' target(s): -Og, debug symbolic info (-g -ggdb), not stripped

#--- Compilation rules for object files
%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.dbg.o: %.c
	${CC} ${CFLAGS_DBG} -c $< -o $@

%.asan.o: %.c
	${CC} ${CFLAGS_DBG_ASAN} -c $< -o $@

%.ub.o: %.c
	${CC} ${CFLAGS_DBG_UB} -c $< -o $@

%.lsan.o: %.c
	${CC} ${CFLAGS_DBG_LSAN} -c $< -o $@

%.msan.o: %.c
	${CC} ${CFLAGS_DBG_MSAN} -c $< -o $@

%.tsan.o: %.c
	${CC} ${CFLAGS_DBG_TSAN} -c $< -o $@

%.gcov.o: %.c
	${CC} ${CFLAGS_GCOV} -c $< -o $@

#--- Generate object file lists for different builds
OBJS_READER_DBG := $(SRCS_READER:.c=.dbg.o)
OBJS_WRITER_DBG := $(SRCS_WRITER:.c=.dbg.o)
OBJS_READER_ASAN := $(SRCS_READER:.c=.asan.o)
OBJS_WRITER_ASAN := $(SRCS_WRITER:.c=.asan.o)
OBJS_READER_UB := $(SRCS_READER:.c=.ub.o)
OBJS_WRITER_UB := $(SRCS_WRITER:.c=.ub.o)
OBJS_READER_LSAN := $(SRCS_READER:.c=.lsan.o)
OBJS_WRITER_LSAN := $(SRCS_WRITER:.c=.lsan.o)
OBJS_READER_MSAN := $(SRCS_READER:.c=.msan.o)
OBJS_WRITER_MSAN := $(SRCS_WRITER:.c=.msan.o)
OBJS_READER_TSAN := $(SRCS_READER:.c=.tsan.o)
OBJS_WRITER_TSAN := $(SRCS_WRITER:.c=.tsan.o)
OBJS_READER_GCOV := $(SRCS_READER:.c=.gcov.o)
OBJS_WRITER_GCOV := $(SRCS_WRITER:.c=.gcov.o)

prod: ${OBJS_READER} ${OBJS_WRITER}
	@echo
	@echo -n "\e[1m\e[41m"
	@echo "--- building 'production'-ready targets (-O${PROD_OPTLEVEL}, no debug, stripped) ---"
	@echo " glibc (and NPTL) version: $(shell getconf GNU_LIBPTHREAD_VERSION|cut -d' ' -f2)"
	@echo -n "\e[0m"
	@echo
	${CC} ${CFLAGS} ${OBJS_READER} -o ${PROG_READER} ${LDFLAGS}
	${STRIP} $(STRIP_SPEC) ./${PROG_READER}
	${CC} ${CFLAGS} ${OBJS_WRITER} -o ${PROG_WRITER} ${LDFLAGS}
	${STRIP} $(STRIP_SPEC) ./${PROG_WRITER}

# The '2-part executable' solution : use strip and objcopy to generate a
# binary executable that has the ability to retrieve debug symbolic information
# from the 'debug' binary!
prod_2part: ${OBJS_READER} ${OBJS_WRITER}
	@echo
	@echo -n "\e[1m\e[41m"
	@echo "--- building 'production'-ready 2-part targets (-O${PROD_OPTLEVEL}, no debug, strip-debug) ---"
	@echo " glibc (and NPTL) version: $(shell getconf GNU_LIBPTHREAD_VERSION|cut -d' ' -f2)"
	@echo -n "\e[0m"
	@echo
# We require the 'debug' build for the 2part, so do that first
	make --ignore-errors debug
	${CC} ${CFLAGS} ${OBJS_READER} -o ${PROG_READER} ${LDFLAGS}
	${STRIP} $(STRIP_SPEC) ./${PROG_READER}
	${OBJCOPY} --add-gnu-debuglink=./${PROG_READER}_dbg ./${PROG_READER}
	${CC} ${CFLAGS} ${OBJS_WRITER} -o ${PROG_WRITER} ${LDFLAGS}
	${STRIP} $(STRIP_SPEC) ./${PROG_WRITER}
	${OBJCOPY} --add-gnu-debuglink=./${PROG_WRITER}_dbg ./${PROG_WRITER}
# verify it's setup
	${READELF} --debug-dump ./${PROG_READER} | grep -A2 "debuglink"
	${READELF} --debug-dump ./${PROG_WRITER} | grep -A2 "debuglink"

# 'debug' target: -Og, debug symbolic info (-g -ggdb), not stripped.
# Generates the regular debug build, debug+ASAN, debug+UB, debug+LSAN, debug+MSAN builds'
# MSAN requires clang.
# When using clang on Debian/Fedora-type distros, use -static-libsan (LINKIN is
# set to this value); with GCC, and on Fedora-type distros, other libraries
# (libasan, libubsan, liblsan) are required (pkg: lib<name>-<gcc ver>-...)
debug: ${OBJS_READER_DBG} ${OBJS_WRITER_DBG} ${OBJS_READER_ASAN} ${OBJS_WRITER_ASAN} ${OBJS_READER_UB} ${OBJS_WRITER_UB} ${OBJS_READER_LSAN} ${OBJS_WRITER_LSAN} ${OBJS_READER_MSAN} ${OBJS_WRITER_MSAN} ${OBJS_READER_TSAN} ${OBJS_WRITER_TSAN}
	@echo
	@echo -n "\e[1m\e[41m"
	@echo "--- building 'debug'-ready targets (with debug symbolic info, not stripped) ---"
	@echo " glibc (and NPTL) version: $(shell getconf GNU_LIBPTHREAD_VERSION|cut -d' ' -f2)"
	@echo -n "\e[0m"
	@echo
	${CC} ${CFLAGS_DBG} ${OBJS_READER_DBG} -o ${PROG_READER}_dbg ${LDFLAGS}
	${CC} ${CFLAGS_DBG} ${OBJS_WRITER_DBG} -o ${PROG_WRITER}_dbg ${LDFLAGS}
#-- Sanitizers (use clang or GCC)
	${CC} ${CFLAGS_DBG_ASAN} ${OBJS_READER_ASAN} -o ${PROG_READER}_dbg_asan ${LDFLAGS} ${LINKIN}
	${CC} ${CFLAGS_DBG_ASAN} ${OBJS_WRITER_ASAN} -o ${PROG_WRITER}_dbg_asan ${LDFLAGS} ${LINKIN}
	${CC} ${CFLAGS_DBG_UB} ${OBJS_READER_UB} -o ${PROG_READER}_dbg_ub ${LDFLAGS} ${LINKIN}
	${CC} ${CFLAGS_DBG_UB} ${OBJS_WRITER_UB} -o ${PROG_WRITER}_dbg_ub ${LDFLAGS} ${LINKIN}
# GCC doesn't support MSAN, clang does
	@echo -n "\e[1m\e[41m"
	@echo "=== ALERT/FYI: GCC doesn't support MSAN, clang does ==="
	@echo -n "\e[0m"
	${CC} ${CFLAGS_DBG_LSAN} ${OBJS_READER_LSAN} -o ${PROG_READER}_dbg_lsan ${LDFLAGS} ${LINKIN}
	${CC} ${CFLAGS_DBG_LSAN} ${OBJS_WRITER_LSAN} -o ${PROG_WRITER}_dbg_lsan ${LDFLAGS} ${LINKIN}
# ThreadSanitizer (TSan):
# For clang ver < 18.1.0 (Mar '24), need to set vm.mmap_rnd_bits sysctl to 28 (default is 32)
# else it bombs on execution (ref: https://stackoverflow.com/a/77856955/779269)
# (The leading hyphen ensures that make doesn't abort on error.)
	-sudo sysctl vm.mmap_rnd_bits=28
	${CC} ${CFLAGS_DBG_MSAN} ${OBJS_READER_MSAN} -o ${PROG_READER}_dbg_msan ${LDFLAGS} ${LINKIN}
	${CC} ${CFLAGS_DBG_MSAN} ${OBJS_WRITER_MSAN} -o ${PROG_WRITER}_dbg_msan ${LDFLAGS} ${LINKIN}
	${CC} ${CFLAGS_DBG_TSAN} ${OBJS_READER_TSAN} -o ${PROG_READER}_dbg_tsan ${LDFLAGS} ${LINKIN}
	${CC} ${CFLAGS_DBG_TSAN} ${OBJS_WRITER_TSAN} -o ${PROG_WRITER}_dbg_tsan ${LDFLAGS} ${LINKIN}


#--------------- More (useful) targets! -------------------------------

# indent- "beautifies" C code - to conform to the the Linux kernel
# coding style guidelines.
# Note! original source file(s) is overwritten, so we back it up.
# code-style : "wrapper" target over the following kernel code style targets
code-style:
	make --ignore-errors indent
	make --ignore-errors checkpatch

indent: ${SRC_FILES}
ifeq (, $(shell which ${INDENT}))
	$(warning === WARNING! ${INDENT} not installed; consider doing 'sudo apt install indent' or equivalent ===)
else
	make clean
	@echo -n "\e[1m\e[41m"
	@echo "--- applying Linux kernel code-style indentation with indent ---"
	@echo -n "\e[0m"
	mkdir bkp 2>/dev/null; cp -f ${SRC_FILES} bkp/
	-${INDENT} -linux ${SRC_FILES}
endif
# RELOOK
# !WARNING!
# I came across this apparent bug in indent when using it on Ubuntu 20.04:
#  realloc(): invalid next size
#  Aborted (core dumped)
# Worse, it TRUNCATED the source file to 0 bytes !!! So backing them up - as we
# indeed do - is good.

checkpatch:
	make clean
	@echo -n "\e[1m\e[41m"
	@echo "--- applying Linux kernel code-style checking with checkpatch.pl ---"
	@echo -n "\e[0m"
	-${CHECKPATCH} -f --no-tree --max-line-length=95 ${SRC_FILES}

# sa : "wrapper" target over the following static analyzer targets
sa:   # static analysis
	make --ignore-errors sa_clangtidy
	make --ignore-errors sa_flawfinder
	make --ignore-errors sa_cppcheck

# static analysis with clang-tidy
sa_clangtidy:
ifeq (, $(shell which ${CLANGTIDY}))
	$(warning === WARNING! ${CLANGTIDY} not installed; consider doing 'sudo apt install clang-tidy' or equivalent ===)
else
	make clean
	@echo -n "\e[1m\e[41m"
	@echo "--- static analysis with clang-tidy ---"
	@echo -n "\e[0m"
	-CHECKS_ON="-*,clang-analyzer-*,bugprone-*,cert-*,concurrency-*,performance-*,portability-*,linuxkernel-*,readability-*,misc-*"; CHECKS_OFF="-clang-analyzer-cplusplus*,-misc-include-cleaner,-readability-identifier-length,-readability-braces-around-statements" ; ${CLANGTIDY} -header-filter=.* --use-color *.[ch] -checks=$$CHECKS_ON,$$CHECKS_OFF
endif

# static analysis with flawfinder
sa_flawfinder:
ifeq (, $(shell which ${FLAWFINDER}))
	$(warning === WARNING! ${FLAWFINDER} not installed; consider doing 'sudo apt install flawfinder' or equivalent ===)
else
	make clean
	@echo -n "\e[1m\e[41m"
	@echo "--- static analysis with flawfinder ---"
	@echo -n "\e[0m"
	-${FLAWFINDER} --neverignore --context *.[ch]
endif

# static analysis with cppcheck
sa_cppcheck:
ifeq (, $(shell which ${CPPCHECK}))
	$(warning === WARNING! ${CPPCHECK} not installed; consider doing 'sudo apt install cppcheck' or equivalent ===)
else
	make clean
	@echo -n "\e[1m\e[41m"
	@echo "--- static analysis with cppcheck ---"
	@echo -n "\e[0m"
	-${CPPCHECK} -v --force --enable=all -i bkp/ --suppress=missingIncludeSystem .
endif

# Dynamic Analysis
# dynamic analysis with valgrind
valgrind:
ifeq (, $(shell which ${VALGRIND}))
	$(warning === WARNING! ${VALGRIND} not installed; consider doing 'sudo apt install valgrind' or equivalent ===)
else
	make --ignore-errors debug
	@echo -n "\e[1m\e[41m"
	@echo "--- dynamic analysis with Valgrind memcheck (reader) ---"
	@echo -n "\e[0m"
	@if test -z "${CMDLINE_ARGS}"; then echo -n "\e[1m\e[31m" ; echo "\n@@@ (Possible) Warning: no parameters being passed to the program under test via Valgrind ? @@@\n(FYI, initialize the Makefile variable CMDLINE_ARGS to setup parameters)"; echo -n "\e[0m" ; fi
	-${VALGRIND} --tool=memcheck --trace-children=yes ./${PROG_READER}_dbg ${CMDLINE_ARGS}
endif

# dynamic analysis with the Sanitizer tooling
san:
	make --ignore-errors debug
	@echo -n "\e[1m\e[41m"
	@echo "--- dynamic analysis with the Address Sanitizer (ASAN) - reader ---"
	@echo -n "\e[0m"
	@if test -z "${CMDLINE_ARGS}"; then echo -n "\e[1m\e[31m" ; echo "\n@@@ (Possible) Warning: no parameters being passed to the program under test ${PROG_READER}_dbg_asan ? @@@\n(FYI, initialize the Makefile variable CMDLINE_ARGS to setup parameters)"; echo -n "\e[0m" ; fi
	-./${PROG_READER}_dbg_asan ${CMDLINE_ARGS}

	@echo -n "\e[1m\e[41m"
	@echo "--- dynamic analysis with the Undefined Behavior Sanitizer (UBSAN) - reader ---"
	@echo -n "\e[0m"
	-./${PROG_READER}_dbg_ub ${CMDLINE_ARGS}

	@echo -n "\e[1m\e[41m"
	@echo "--- dynamic analysis with the Memory Sanitizer (MSAN) - reader ---"
	@echo -n "\e[0m"
	-./${PROG_READER}_dbg_msan ${CMDLINE_ARGS}

	@echo -n "\e[1m\e[41m"
	@echo "--- dynamic analysis with the Thread Sanitizer (TSAN) - reader ---"
	@echo -n "\e[0m"
	-./${PROG_READER}_dbg_tsan ${CMDLINE_ARGS}

# dynamic analysis run with LSan binary not done here (as ASan typically covers leakage)

#----- Testing: line coverage with gcov(1), lcov(1)
# ref: https://backstreetcoder.com/code-coverage-using-gcov-lcov-in-linux/
covg:
	@echo -n "\e[1m\e[41m"
	@echo "=== Code coverage (funcs/lines/branches) with gcov+lcov ==="
	@echo -n "\e[0m"

ifeq (,$(wildcard /etc/lcovrc))
	$(error ERROR: install lcov first)
endif
# Set up the ~/.lcovrc to include branch coverage
# ref: https://stackoverflow.com/questions/12360167/generating-branch-coverage-data-for-lcov
ifneq (,$(wildcard ~/.lcovrc))
	@echo "~/.lcovrc in place"
else
	cp /etc/lcovrc ~/.lcovrc
	sed -i 's/^#genhtml_branch_coverage = 1/genhtml_branch_coverage = 1/' ~/.lcovrc
	sed -i 's/^lcov_branch_coverage = 0/lcov_branch_coverage = 1/' ~/.lcovrc
endif
ifeq (, $(shell which ${LCOV_SCRIPT}))
	$(error ERROR: ensure our ${LCOV_SCRIPT} wrapper script's installed and in your PATH first; location: https://github.com/kaiwan/usefulsnips/blob/master/lcov_gen.sh)
endif

#--- Build for coverage testing; this generates the binary executables named
# ${PROG_READER}_covg and ${PROG_WRITER}_covg and the .gcno ('notes') files as well
	make clean
	@echo "___"
	@echo -n "\e[1m\e[41m"
	@echo "> Forcing compiler to GCC for coverage, as gcov/lcov seem to require it"
	@echo -n "\e[0m"
	$(eval CC := gcc)
# For coverage analysis, gcov/lcov seems to require compilation via GCC (not clang)
	${CC} ${CFLAGS_GCOV} ${OBJS_READER_GCOV} -o ${PROG_READER}_gcov ${LDFLAGS}
	${CC} ${CFLAGS_GCOV} ${OBJS_WRITER_GCOV} -o ${PROG_WRITER}_gcov ${LDFLAGS}
	@if test -z "${CMDLINE_ARGS}"; then echo "\n@@@ (Possible) Warning: no parameters being passed to the program under test ${PROG_READER}_gcov ? @@@\n(FYI, initialize the Makefile variable CMDLINE_ARGS to setup parameters)"; fi

	@echo -n "\e[1m\e[41m"
	@echo "-------------- Running via our wrapper ${LCOV_SCRIPT} (reader) --------------"
	@echo -n "\e[0m"
	-${LCOV_SCRIPT} ${PROG_READER}_gcov ${CMDLINE_ARGS}
# lcov_gen.sh Notes:
#  - If you want a cumulative / merged code coverage report, run your next coverage
#    test case via this script. In effect, simply adjust the CMDLINE_ARGS variable here
#    and run 'make covg' again
#  - If you want to start from scratch, *wiping* previous coverage data, then
#    add the -r (reset) option when invoking this script (above) -OR-
#    simply invoke the 'clean_lcov' target (which deletes all the lcov meta dirs)

# Security
checksec:
ifeq (, $(shell which ${CHECKSEC} 2>/dev/null))
   $(warning === WARNING! ${CHECKSEC} not installed; consider doing 'sudo apt install checksec' or equivalent ===)
else
	make prod_2part
	checksec --file=$(PROG_READER)
	checksec --file=$(PROG_WRITER)
endif

### More targets? Add them below


# exit unconditionally
%:
	@true

# Testing all
# Limitation:
# When the PUT (Prg Under Test) runs in an infinite loop or forever (eg. servers/daemons),
# you may have to manually run a client process (or whatever) and exit the main process
# programatically; else, a signal like ^C does abort it BUT make doesn't continue (even
# when run with --ignore-errors).
test:
	@echo
	@echo -n "\e[1m\e[41m"
	@echo "=== Test All ==="
	@echo -n "\e[0m"
	@echo "-------------------------------------------------------------------------------"
	-make --ignore-errors code-style
	@echo "-------------------------------------------------------------------------------"
	-make --ignore-errors sa
	@echo "-------------------------------------------------------------------------------"
	-make --ignore-errors valgrind
	@echo "-------------------------------------------------------------------------------"
	-make --ignore-errors san
	@echo "-------------------------------------------------------------------------------"
	-make --ignore-errors covg
	@echo "-------------------------------------------------------------------------------"
	-make --ignore-errors checksec

# packaging
package:
	@echo -n "\e[1m\e[41m"
	@echo "--- packaging ---"
	@echo -n "\e[0m"
	rm -f ../${PKG_NAME}.tar.xz
	make clean
	tar caf ../${PKG_NAME}.tar.xz *
	ls -l ../${PKG_NAME}.tar.xz
	@echo "=== $(PKG_NAME).tar.xz package created ==="
	@echo 'Tip: when extracting, to extract into a dir of the same name as the tar file,'
	@echo ' do: tar -xvf ${PKG_NAME}.tar.xz --one-top-level'

clean:
	@echo -n "\e[1m\e[41m"
	@echo "--- cleaning ---"
	@echo -n "\e[0m"
	rm -vf ${ALL_NM} core* vgcore* *.o *.dbg.o *.asan.o *.ub.o *.lsan.o *.msan.o *.tsan.o *.gcov.o *~
# rm some of the code coverage metadata
	rm -rfv ${PROG_READER}_gcov ${PROG_WRITER}_gcov *.[ch].gcov *.gcda *.gcno *.info

	@if [ -d lcov_onerun_html ]; then \
	  echo "------------------- NOTE: clean for lcov (covg target) ----------------------------" ;\
	  echo "Special case wrt the 'clean' target and the code coverage target (covg):" ;\
	  echo " It deliberately does NOT delete the LCOV metadata, intermediate and final LCOV coverage" ;\
	  echo " report folders - the ones named 0lcov_meta/, lcov_onerun_html/ and lcov_merged_html/ resp," ;\
	  echo " as they're required to generate a merged or cumulative code coverage report." ;\
	  echo "So: to start code coverage analysis from scratch, you can either:" ;\
	  echo "- Invoke the special 'make clean_lcov' (it manually delete these 3 folders)" ;\
	  echo "  OR" ;\
	  echo "- Change the invocation of the lcov_gen.sh script in the Makefile, passing the -r option" ;\
	  echo "-----------------------------------------------------------------------------------" ;\
	fi

clean_lcov:
	@echo -n "\e[1m\e[41m"
	@echo "--- cleaning LCOV metadata ---"
	@echo -n "\e[0m"
	# NOTE! depedency on the lcov_gen.sh script, on these folder names
	rm -rvf 0lcov_meta/  lcov_merged_html/  lcov_onerun_html/

help:
	@echo -n "\e[1m\e[41m"
	@echo '=== Makefile Help : additional targets available ==='
	@echo -n "\e[0m"
	@echo
	@echo 'This Makefile supports building C programs with multiple independent executables.'
	@echo 'It is currently configured to build two programs: reader and writer'
	@echo 'To customize:'
	@echo '  1. Set PROG_READER and PROG_WRITER to your program names'
	@echo '  2. Set SRCS_READER and SRCS_WRITER to their respective source files'
	@echo '     Example: SRCS_READER := main.c helper.c'
	@echo '             SRCS_WRITER := writer.c helper.c'
	@echo '  3. Object files are automatically compiled separately for each program'
	@echo
	@echo 'For single-program projects, modify accordingly and define a single set of variables.'
	@echo
	@echo 'TIP: type make <tab><tab> to show all valid targets'
	@echo

	@echo -n "\e[1m\e[34m"
	@echo 'Regular targets ::'
	@echo -n "\e[0m"
	@echo ' 1. 'prod'  : regular production target: -O${PROD_OPTLEVEL}, no debug symbolic info, stripped'
	@echo ' 2. 'debug' : -Og, debug symbolic info (-g -ggdb), not stripped. Generates the regular debug build, debug+ASAN, debug+UB, debug+LSAN, debug+MSAN builds'
	@echo ' 3. 'prod_2part': production target : -O${PROD_OPTLEVEL}, no debug symbolic info, strip-debug; \
    Excellent for production as it gives ability to debug as and when required! \
    (shown as third option as it *requires* the 'debug' build as a step'
	@echo
	@echo 'Doing a 'make' will build all three shown above.'

	@echo
	@echo -n "\e[1m\e[34m"
	@echo '--- code style targets ---'
	@echo -n "\e[0m"
	@echo 'code-style : "wrapper" target over the following kernel code style targets'
	@echo ' indent     : run the $(INDENT) utility on source file(s) to indent them as per the kernel code style'
	@echo ' checkpatch : run the kernel code style checker tool on source file(s)'

	@echo
	@echo -n "\e[1m\e[34m"
	@echo '--- static analyzer targets ---'
	@echo -n "\e[0m"
	@echo 'sa          : "wrapper" target over the following static analyzer targets'
	@echo ' sa_clangtidy  : run the static analysis clang-tidy tool on the source file(s)'
	@echo ' sa_flawfinder : run the static analysis flawfinder tool on the source file(s)'
	@echo ' sa_cppcheck   : run the static analysis cppcheck tool on the source file(s)'

	@echo
	@echo -n "\e[1m\e[34m"
	@echo '--- dynamic analysis targets ---'
	@echo -n "\e[0m"
	@echo ' valgrind   : run the dynamic analysis tool ($(VALGRIND)) on the binary executable'
	@echo ' san        : run dynamic analysis via ASAN, UBSAN, MSAN and TSAN tooling on the binary executable'

	@echo
	@echo -n "\e[1m\e[34m"
	@echo '--- code coverage ---'
	@echo -n "\e[0m"
	@echo ' covg       : run the gcov+lcov code coverage tooling on the source (generates html output!). NOTE: this target requires our ${LCOV_SCRIPT} wrapper script installed (location: https://github.com/kaiwan/usefulsnips/blob/master/lcov_gen.sh)'
	@echo " Note: Special case wrt the 'clean' target and the code coverage target (covg):"
	@echo "  It deliberately does NOT delete the LCOV metadata, intermediate and final LCOV coverage"
	@echo "  report folders - the ones named 0lcov_meta/, lcov_onerun_html/ and lcov_merged_html/ resp,"
	@echo "  as they're required to generate a merged / cumulative code coverage report."
	@echo " To start code coverage analysis from scratch, you can either:"
	@echo " - invoke the special 'make clean_lcov' (it manually delete these 3 folders), OR,"
	@echo " - change the invocation of the lcov_gen.sh script in the Makefile, passing the -r option"

	@echo
	@echo -n "\e[1m\e[34m"
	@echo '--- security: checksec target ---'
	@echo -n "\e[0m"
	@echo ' checksec   : run the checksec Bash script on the binary executable'

	@echo
	@echo -n "\e[1m\e[34m"
	@echo '--- TEST all ---'
	@echo -n "\e[0m"
	@echo ' test       : run all targets (it runs them in this order): code-style, sa, valgrind, san, covg'
	@echo '              Tip: run "make -i test > out 2>&1" to save all output to a file "out".'
	@echo '                             -i = --ignore-errors'

	@echo
	@echo -n "\e[1m\e[34m"
	@echo '--- misc targets ---'
	@echo -n "\e[0m"
	@echo ' clean      : cleanup - remove all the binaries, core files, etc'
	@echo '              See special note wrt code coverage'
	@echo ' clean_lcov : cleanup the LCOV metadata folders; implies code coverage starts from scratch'
	@echo ' package    : tar and compress the source files into the dir above'
	@echo '  Tip: when extracting, to extract into a dir of the same name as the tar file, do:'
	@echo '       tar -xvf ${PKG_NAME}.tar.xz --one-top-level'

	@echo ' help       : this 'help' target'
