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

#----------------- User configurable section ---------------------------------
# Update as required
# Set the program name (executable name) here:
PROG_NAME := demo1

# List all C source files (can be in subdirectories or same directory)
# Examples:
#  SRCS := main.c helper.c utils.c
#  SRCS := src/main.c src/helper.c src/utils.c
# If appropriate, can do:
#SRCS := $(wildcard *.c)
SRCS := main.c s2.c audio/s4.c engine/s3.c

# Verbosity in builds
VERBOSE ?= 1  # set to 0 for disabling verbose builds; or run as 'make V=0'

#--- CHECK: manually add params as required
# Populate any required cmdline arguments to the process here:
CMDLINE_ARGS=""
#----------------- END User configurable section ------------------------------
#----------------- Typically no need to change anything below this line -------

# Automatically generate object file names from source files
# Build directory for object and intermediate files (out-of-source build)
# You can override on the make command line: `make OBJDIR=build`.
OBJDIR ?= build

# Automatically generate object file names from source files (preserve subdirs under OBJDIR)
OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

# For coverage builds, we need alternate object file names
OBJS_GCOV := $(patsubst %.c,$(OBJDIR)/%.gcov.o,$(SRCS))

# Verbose toggle: set V=0 or VERBOSE=0 to disable verbose builds
V ?= 1
ifeq ($(VERBOSE),1)
V := 1
endif
ifeq ($(V),1)
Q :=
else
Q := @
endif

# ANSI color sequences for pretty messages (use with printf '%b')
BOLD_ESC := $(shell printf '\\033')
ESC := $(BOLD_ESC)
BOLD := $(ESC)[1m
RED := $(ESC)[31m
GREEN := $(ESC)[32m
YELLOW := $(ESC)[33m
BLUE := $(ESC)[34m
BG_RED := $(ESC)[41m
RESET := $(ESC)[0m

# Can remove the 'msan' one if it doesn't build - requires clang
ALL_NM :=  ${PROG_NAME} ${PROG_NAME}_dbg ${PROG_NAME}_dbg_asan ${PROG_NAME}_dbg_ub ${PROG_NAME}_dbg_lsan ${PROG_NAME}_gcov ${PROG_NAME}_dbg_msan ${PROG_NAME}_dbg_tsan

# Decide which compiler to use; GCC doesn't support MSAN, clang does
CC := ${CROSS_COMPILE}gcc
LINKIN := -static-libasan   # use this lib for ASAN with GCC
ifeq (, $(shell which clang))
	@printf '%b' '$(BOLD)$(BG_RED)'
	$(warning === WARNING! No clang (compiler) in PATH (reqd for MSAN); consider doing 'sudo apt install clang' or equivalent ===)
	@printf '%b' '$(RESET)'
else
	CC := clang
	LINKIN := -static-libsan
endif
# If required, the below 2 lines can be uncommented to force GCC as the compiler
#CC := ${CROSS_COMPILE}gcc
#LINKIN := -static-libasan
## Compiler info will be printed at runtime by the `all` recipe

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
  # -O<N>; for production N is typically 1, 2, 3 or s (optimize for speed & size);
  #  in the -D_FORTIFY_SOURCE=N option, you must set N to the same number
DEBUG_OPTLEVEL=0
  # g
  # -O<N>; for debug N is typically 0 or g; see comments below re: -Og vs -O0

#--- Security-related compiler flags !
# Ref: GCC security related flags reference [https://gist.github.com/kaiwan/d82d8271459c90c3accc3f795dcaf228]
SECURITY_CFLAGS_REG = -fstack-protector-strong \
	-pie -fPIE \
	-fsanitize=bounds -fsanitize-undefined-trap-on-error \
	-fcf-protection \
	-fsanitize=signed-integer-overflow -fsanitize-undefined-trap-on-error
#	-D_FORTIFY_SOURCE=${PROD_OPTLEVEL}
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
## CFLAGS printed at runtime by the `all` recipe
CFLAGS_DBG = -DDEBUG -g -ggdb -gdwarf-4 -O$(DEBUG_OPTLEVEL) \
	$(WARN) ${WARN_MORE} -fno-omit-frame-pointer \
	${CSTD} -D_POSIX_C_SOURCE=${POSIX_STD} ${STD_DEFS} \
	-pie -fPIE \
	-fsanitize=bounds -fsanitize-undefined-trap-on-error \
	-fcf-protection \
	-fsanitize=signed-integer-overflow -fsanitize-undefined-trap-on-error
#	${SECURITY_CFLAGS}
## CFLAGS_DBG printed at runtime by the `all` recipe

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

# Install locations (can be overridden by caller)
prefix ?= /usr/local
bindir ?= $(prefix)/bin
sysconfdir ?= $(prefix)/etc
DESTDIR ?=
# For Windows add:
# -Wl,dynamicbase : tell linker to use ASLR protection
# -Wl,nxcompat : tell linker to use DEP protection
.DEFAULT_GOAL := all
.PHONY: all prod prod_2part debug covg clean clean_lcov help \
	install uninstall code-style indent checkpatch sa sa_clangtidy sa_flawfinder sa_cppcheck \
	valgrind san test package
all:
	@printf '%b\n' '$(BOLD)$(GREEN)Compiler    = $(CC)$(RESET)'
	@printf '%b\n' '$(BOLD)$(BLUE)LDFLAGS     = $(LDFLAGS)$(RESET)'
	@printf '%b\n' '$(BOLD)$(GREEN)CFLAGS      = $(CFLAGS)$(RESET)'
	@printf '%b\n' '$(BOLD)$(BLUE)CFLAGS_DBG  = $(CFLAGS_DBG)$(RESET)'
	@printf '%b\n\n' '$(BOLD)$(GREEN)Verbosity   = $(VERBOSE)$(RESET)'
	@$(MAKE) ${ALL}

# glibc >= 2.34, libpthread is a part of it; hence were not required to link
# with -pthread any longer
#LINK=  #-pthread

# Required vars
SRC_FILES := *.[ch]
INDENT := indent
CLANGTIDY := clang-tidy
FLAWFINDER := flawfinder
CPPCHECK := cppcheck
VALGRIND := valgrind
# update as required
PKG_NAME := ${PROG_NAME}
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
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(Q)${CC} ${CFLAGS} -c $< -o $@

$(OBJDIR)/%.dbg.o: %.c
	@mkdir -p $(dir $@)
	$(Q)${CC} ${CFLAGS_DBG} -c $< -o $@

$(OBJDIR)/%.asan.o: %.c
	@mkdir -p $(dir $@)
	$(Q)${CC} ${CFLAGS_DBG_ASAN} -c $< -o $@

$(OBJDIR)/%.ub.o: %.c
	@mkdir -p $(dir $@)
	$(Q)${CC} ${CFLAGS_DBG_UB} -c $< -o $@

$(OBJDIR)/%.lsan.o: %.c
	@mkdir -p $(dir $@)
	$(Q)${CC} ${CFLAGS_DBG_LSAN} -c $< -o $@

$(OBJDIR)/%.tsan.o: %.c
	@mkdir -p $(dir $@)
	$(Q)${CC} ${CFLAGS_DBG_TSAN} -c $< -o $@

$(OBJDIR)/%.gcov.o: %.c
	@mkdir -p $(dir $@)
	$(Q)${CC} ${CFLAGS_GCOV} -c $< -o $@

#--- Generate object file lists for different builds
OBJS_DBG := $(patsubst %.c,$(OBJDIR)/%.dbg.o,$(SRCS))
OBJS_ASAN := $(patsubst %.c,$(OBJDIR)/%.asan.o,$(SRCS))
OBJS_UB := $(patsubst %.c,$(OBJDIR)/%.ub.o,$(SRCS))
OBJS_LSAN := $(patsubst %.c,$(OBJDIR)/%.lsan.o,$(SRCS))
## MSAN builds are handled explicitly in the debug recipe (only when using clang)
OBJS_TSAN := $(patsubst %.c,$(OBJDIR)/%.tsan.o,$(SRCS))
OBJS_GCOV := $(patsubst %.c,$(OBJDIR)/%.gcov.o,$(SRCS))


prod: ${OBJS}
	@printf '\n'
	@printf '%b\n' '$(BOLD)$(BG_RED)--- Building production-ready target (-O${PROD_OPTLEVEL}, no debug, stripped) ---$(RESET)'
	@printf '%s\n' 'NPTL version: $(shell getconf GNU_LIBPTHREAD_VERSION|cut -d' ' -f2)'
	@printf '\n'
	$(Q)${CC} ${CFLAGS} ${OBJS} -o ${PROG_NAME} ${LDFLAGS}
	$(Q)${STRIP} $(STRIP_SPEC) ./${PROG_NAME}

# The '2-part executable' solution : use strip and objcopy to generate a
# binary executable that has the ability to retrieve debug symbolic information
# from the 'debug' binary!
prod_2part: ${OBJS}
	@printf '\n'
	@printf '%b\n' '$(BOLD)$(BG_RED)--- Building production-ready 2-part target (-O${PROD_OPTLEVEL}, no debug, strip-debug) ---$(RESET)'
	@printf '%s\n' " glibc (and NPTL) version: $(shell getconf GNU_LIBPTHREAD_VERSION|cut -d' ' -f2)"
	@printf '\n'
# We require the 'debug' build for the 2part, so do that first
	make --ignore-errors debug
	$(Q)${CC} ${CFLAGS} ${OBJS} -o ${PROG_NAME} ${LDFLAGS}
	$(Q)${STRIP} $(STRIP_SPEC) ./${PROG_NAME}
# Most IMP, setup the 'debug link' in the release binary, pointing to the debug
# info file; this, in effect, is the 2part solution!
# quiet-friendly
	$(Q)${OBJCOPY} --add-gnu-debuglink=./${PROG_NAME}_dbg ./${PROG_NAME}
# verify it's setup
	$(Q)${READELF} --debug-dump ./${PROG_NAME} | grep -A2 "debuglink"

# 'debug' target: -Og, debug symbolic info (-g -ggdb), not stripped.
# Generates the regular debug build, debug+ASAN, debug+UB, debug+LSAN, debug+MSAN builds'
# MSAN requires clang.
# When using clang on Debian/Fedora-type distros, use -static-libsan (LINKIN is
# set to this value); with GCC, and on Fedora-type distros, other libraries
# (libasan, libubsan, liblsan) are required (pkg: lib<name>-<gcc ver>-...)


debug: ${OBJS_DBG} ${OBJS_ASAN} ${OBJS_UB} ${OBJS_LSAN} ${OBJS_TSAN}
	@printf '\n'
	@printf '%b\n' '$(BOLD)$(BG_RED)--- Building debug-ready targets (with debug symbolic info, not stripped) ---$(RESET)'
	@printf '%s\n' " glibc (and NPTL) version: $(shell getconf GNU_LIBPTHREAD_VERSION|cut -d' ' -f2)"
	@printf '\n'
# build debug binary
	$(Q)${CC} ${CFLAGS_DBG} ${OBJS_DBG} -o ${PROG_NAME}_dbg ${LDFLAGS}
#-- Sanitizers (use clang or GCC)
# asan/ubsan builds
	$(Q)${CC} ${CFLAGS_DBG_ASAN} ${OBJS_ASAN} -o ${PROG_NAME}_dbg_asan ${LDFLAGS} ${LINKIN}
	$(Q)${CC} ${CFLAGS_DBG_UB} ${OBJS_UB} -o ${PROG_NAME}_dbg_ub ${LDFLAGS} ${LINKIN}
# GCC doesn't support MSAN, clang does
	# Build MSAN debug binary only when using clang; compile directly from sources
	$(Q)if echo "$(CC)" | grep -q clang; then \
		$(CC) ${CFLAGS_DBG_MSAN} $(SRCS) -o ${PROG_NAME}_dbg_msan ${LDFLAGS} ${LINKIN}; \
	else \
		printf '%b\n' '--- MSAN build skipped (CC is not clang, gcc does not support MSAN) ---'; \
	fi
# lsan build
	$(Q)${CC} ${CFLAGS_DBG_LSAN} ${OBJS_LSAN} -o ${PROG_NAME}_dbg_lsan ${LDFLAGS} ${LINKIN}
# ThreadSanitizer (TSan):
# For clang ver < 18.1.0 (Mar '24), need to set vm.mmap_rnd_bits sysctl to 28 (default is 32)
# else it bombs on execution (ref: https://stackoverflow.com/a/77856955/779269)
# (The leading hyphen ensures that make doesn't abort on error.)
	# Only set vm.mmap_rnd_bits when using clang versions < 18.1.0
	@if echo "$(CC)" | grep -q clang; then \
		ver_line="$$( $(CC) --version | head -n1 )"; \
		maj=$$(echo "$$ver_line" | sed -E 's/.*version[[:space:]]*([0-9]+)\.([0-9]+).*/\1/'); \
		min=$$(echo "$$ver_line" | sed -E 's/.*version[[:space:]]*([0-9]+)\.([0-9]+).*/\2/'); \
		if [ -n "$$maj" ]; then \
			if [ "$$maj" -lt 18 ] || { [ "$$maj" -eq 18 ] && [ "$$min" -lt 1 ]; }; then \
				sudo sysctl vm.mmap_rnd_bits=28 || true; \
			else \
				echo "clang version $$maj.$$min >= 18.1 — skipping vm.mmap_rnd_bits tweak"; \
			fi; \
		else \
			echo "Unable to determine clang version; skipping vm.mmap_rnd_bits tweak"; \
		fi; \
	fi
	$(Q)${CC} ${CFLAGS_DBG_TSAN} ${OBJS_TSAN} -o ${PROG_NAME}_dbg_tsan ${LDFLAGS} ${LINKIN}


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
	@printf '%b\n' '$(BOLD)$(BG_RED)--- applying Linux kernel code-style indentation with indent ---$(RESET)'
	-${INDENT} -linux ${SRC_FILES}
	# Backup files auto-created with ~ suffix
endif

checkpatch:
	make clean
	@printf '%b\n' '$(BOLD)$(BG_RED)--- applying Linux kernel code-style checking with checkpatch.pl ---$(RESET)'
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
	@printf '%b\n' '$(BOLD)$(BG_RED)--- static analysis with clang-tidy ---$(RESET)'
	-CHECKS_ON="-*,clang-analyzer-*,bugprone-*,cert-*,concurrency-*,performance-*,portability-*,linuxkernel-*,readability-*,misc-*"; CHECKS_OFF="-clang-analyzer-cplusplus*,-misc-include-cleaner,-readability-identifier-length,-readability-braces-around-statements" ; ${CLANGTIDY} -header-filter=.* --use-color *.[ch] -checks=$$CHECKS_ON,$$CHECKS_OFF
endif

# static analysis with flawfinder
sa_flawfinder:
ifeq (, $(shell which ${FLAWFINDER}))
	$(warning === WARNING! ${FLAWFINDER} not installed; consider doing 'sudo apt install flawfinder' or equivalent ===)
else
	make clean
	@printf '%b\n' '$(BOLD)$(BG_RED)--- static analysis with flawfinder ---$(RESET)'
	-${FLAWFINDER} --neverignore --context *.[ch]
endif

# static analysis with cppcheck
sa_cppcheck:
ifeq (, $(shell which ${CPPCHECK}))
	$(warning === WARNING! ${CPPCHECK} not installed; consider doing 'sudo apt install cppcheck' or equivalent ===)
else
	make clean
	@printf '%b\n' '$(BOLD)$(BG_RED)--- static analysis with cppcheck ---$(RESET)'
	-${CPPCHECK} -v --force --enable=all -i bkp/ --suppress=missingIncludeSystem .
endif

# Dynamic Analysis
# dynamic analysis with valgrind
valgrind:
ifeq (, $(shell which ${VALGRIND}))
	$(warning === WARNING! ${VALGRIND} not installed; consider doing 'sudo apt install valgrind' or equivalent ===)
else
	make --ignore-errors debug
	@printf '%b\n' '$(BOLD)$(BG_RED)--- dynamic analysis with Valgrind memcheck ---$(RESET)'
#--- CHECK: have you populated the above CMDLINE_ARGS var with the required cmdline?
	@if test -z "${CMDLINE_ARGS}"; then echo -n "\e[1m\e[31m" ; echo "\n@@@ (Possible) Warning: no parameters being passed to the program under test via Valgrind ? @@@\n(FYI, initialize the Makefile variable CMDLINE_ARGS to setup parameters)"; echo -n "\e[0m" ; fi
	-${VALGRIND} --tool=memcheck --trace-children=yes ./${PROG_NAME}_dbg ${CMDLINE_ARGS}
endif

# dynamic analysis with the Sanitizer tooling
san:
	make --ignore-errors debug
	@printf '%b\n' '$(BOLD)$(BG_RED)--- dynamic analysis with the Address Sanitizer (ASAN) ---$(RESET)'
	@if test -z "${CMDLINE_ARGS}"; then echo -n "\e[1m\e[31m" ; echo "\n@@@ (Possible) Warning: no parameters being passed to the program under test ${PROG_NAME}_dbg_asan ? @@@\n(FYI, initialize the Makefile variable CMDLINE_ARGS to setup parameters)"; echo -n "\e[0m" ; fi
	-./${PROG_NAME}_dbg_asan ${CMDLINE_ARGS}

	@printf '%b\n' '$(BOLD)$(BG_RED)--- dynamic analysis with the Undefined Behavior Sanitizer (UBSAN) ---$(RESET)'
	-./${PROG_NAME}_dbg_ub ${CMDLINE_ARGS}

		ifneq (,$(findstring clang,$(CC)))
		@printf '%b\n' '$(BOLD)$(BG_RED)--- dynamic analysis with the Memory Sanitizer (MSAN) ---$(RESET)'
		-./${PROG_NAME}_dbg_msan ${CMDLINE_ARGS}
		endif

	@printf '%b\n' '$(BOLD)$(BG_RED)--- dynamic analysis with the Thread Sanitizer (TSAN) ---$(RESET)'
	-./${PROG_NAME}_dbg_tsan ${CMDLINE_ARGS}

# dynamic analysis run with LSan binary not done here (as ASan typically covers leakage)

#----- Testing: line coverage with gcov(1), lcov(1)
# ref: https://backstreetcoder.com/code-coverage-using-gcov-lcov-in-linux/
covg:
	@printf '%b\n' '$(BOLD)$(BG_RED)=== Code coverage (funcs/lines/branches) with gcov+lcov ===$(RESET)'

ifeq (,$(wildcard /etc/lcovrc))
	$(error ERROR: install lcov first)
endif
# Set up the ~/.lcovrc to include branch coverage
# ref: https://stackoverflow.com/questions/12360167/generating-branch-coverage-data-for-lcov

ifneq (,$(wildcard ~/.lcovrc))
	@printf '%s\n' '~/.lcovrc in place'
else
	cp /etc/lcovrc ~/.lcovrc
	sed -i 's/^#genhtml_branch_coverage = 1/genhtml_branch_coverage = 1/' ~/.lcovrc
	sed -i 's/^lcov_branch_coverage = 0/lcov_branch_coverage = 1/' ~/.lcovrc
endif
ifeq (, $(shell which ${LCOV_SCRIPT}))
	$(error ERROR: ensure our ${LCOV_SCRIPT} wrapper script's installed and in your PATH first; location: https://github.com/kaiwan/usefulsnips/blob/master/lcov_gen.sh)
endif

#--- Build for coverage testing; this generates the binary executable named
# ${PROG_NAME}_covg and the .gcno ('notes') files as well
	make clean
	@printf '%s\n' '___'
	@printf '%b\n' '$(BOLD)$(BG_RED)> Forcing compiler to GCC for coverage, as gcov/lcov seem to require it$(RESET)'
	$(eval CC := gcc)
	# For coverage analysis, gcov/lcov seems to require compilation via GCC (not clang)
	# Ensure coverage object files are built first
	# remove any existing coverage object files so they are rebuilt with GCC
	@rm -f ${OBJS_GCOV} *.gcno *.gcda || true
	# build coverage object files using the forced CC (pass CC into the sub-make)
	$(MAKE) CC=$(CC) ${OBJS_GCOV}
	${CC} ${CFLAGS_GCOV} ${OBJS_GCOV} -o ${PROG_NAME}_gcov ${LDFLAGS}
	@if test -z "${CMDLINE_ARGS}"; then echo "\n@@@ (Possible) Warning: no parameters being passed to the program under test ${PROG_NAME}_gcov ? @@@\n(FYI, initialize the Makefile variable CMDLINE_ARGS to setup parameters)"; fi

	@printf '%b\n' '$(BOLD)$(BG_RED)-------------- Running via our wrapper ${LCOV_SCRIPT} --------------$(RESET)'
	-${LCOV_SCRIPT} ${PROG_NAME}_gcov ${CMDLINE_ARGS}
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
	checksec --file=$(PROG_NAME)
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
	@printf '\n'
	@printf '%b\n' '$(BOLD)$(BG_RED)=== Test All ===$(RESET)'
	@printf '%s\n' '-------------------------------------------------------------------------------'
	-make --ignore-errors code-style
	@printf '%s\n' '-------------------------------------------------------------------------------'
	-make --ignore-errors sa
	@printf '%s\n' '-------------------------------------------------------------------------------'
	-make --ignore-errors valgrind
	@printf '%s\n' '-------------------------------------------------------------------------------'
	-make --ignore-errors san
	@printf '%s\n' '-------------------------------------------------------------------------------'
	-make --ignore-errors covg
	@printf '%s\n' '-------------------------------------------------------------------------------'
	-make --ignore-errors checksec

# packaging
	package:
		@printf '%b\n' '$(BOLD)$(BG_RED)--- packaging ---$(RESET)'
	rm -f ../${PKG_NAME}.tar.xz
	make clean
	tar caf ../${PKG_NAME}.tar.xz *
	ls -l ../${PKG_NAME}.tar.xz
	@printf '%s\n' '=== $(PKG_NAME).tar.xz package created ==='
	@printf '%s\n' 'Tip: when extracting, to extract into a dir of the same name as the tar file,'
	@printf '%s\n' ' do: tar -xvf ${PKG_NAME}.tar.xz --one-top-level'

# Install / Uninstall targets
install: prod
	@printf '%b\n' '$(BOLD)$(BG_RED)--- Installing ${PROG_NAME} to $(DESTDIR)$(bindir) ---$(RESET)'
	@install -d $(DESTDIR)$(bindir)
	$(Q)install -m 0755 ./${PROG_NAME} $(DESTDIR)$(bindir)/${PROG_NAME}

uninstall:
	@printf '%b\n' '$(BOLD)$(BG_RED)--- Uninstalling ${PROG_NAME} from $(DESTDIR)$(bindir) ---$(RESET)'
	$(Q)rm -fv $(DESTDIR)$(bindir)/${PROG_NAME} || true

clean:
	@printf '%b\n' '$(BOLD)$(BG_RED)--- cleaning ---$(RESET)'
	# remove build artifacts in OBJDIR (out-of-source build)
	rm -rfv $(OBJDIR) || true
	rm -vf ${ALL_NM} core* vgcore* *.o *.dbg.o *.asan.o *.ub.o *.lsan.o *.msan.o *.tsan.o *.gcov.o *~
# rm some of the code coverage metadata
	rm -rfv ${PROG_NAME}_gcov *.[ch].gcov *.gcda *.gcno *.info

	@if [ -d lcov_onerun_html ]; then \
	  echo "------------------- NOTE: clean for lcov (covg target) ----------------------------" ;\
	  echo "Special case wrt the 'clean' target and the code coverage target (covg):" ;\
	  echo " It deliberately does NOT delete the LCOV metadata, intermediate and final LCOV coverage" ;\
	  echo " report folders - the ones named 0lcov_meta/, lcov_onerun_html/ and lcov_merged_html/ resp," ;\
	  echo " as they're required to generate a merged or cumulative code coverage report." ;\
	  echo "So: to start code coverage analysis from scratch, you can either:" ;\
	  echo "- Invoke the special 'make clean_lcov' (it manually deletes these 3 folders)" ;\
	  echo "  OR" ;\
	  echo "- Change the invocation of the lcov_gen.sh script in the Makefile, passing the -r option" ;\
	  echo "-----------------------------------------------------------------------------------" ;\
	fi

clean_lcov:
	make clean
	@printf '%b\n' '$(BOLD)$(BG_RED)--- cleaning LCOV metadata ---$(RESET)'
	# NOTE! depedency on the lcov_gen.sh script, on these folder names
	rm -rvf 0lcov_meta/  lcov_merged_html/  lcov_onerun_html/



help:
	@printf '%b\n' '$(BOLD)$(BG_RED)=== Makefile Help : additional targets available ===$(RESET)'
	@printf '\n'
	@printf '%s\n' 'This Makefile supports building C programs with multiple source files.'
	@printf '%s\n' 'To use:'
	@printf '%s\n' '  1. Set `PROG_NAME` to your program name (default: demo1)'
	@printf '%s\n' '  2. Set `SRCS` to your list of source files (e.g. main.c s2.c audio/s4.c)'
	@printf '%s\n' '  3. Object files are automatically compiled with proper dependencies'
	@printf '\n'
	@printf '%s\n' 'TIP: type make <tab><tab> to show all valid targets'
	@printf '\n'
	@printf '%b\n' '$(BOLD)$(BLUE)Regular targets::$(RESET)'
	@printf '%s\n' "  prod      : production build (-O${PROD_OPTLEVEL}, stripped)"
	@printf '%s\n' "  debug     : debug build (-O${DEBUG_OPTLEVEL}, debug symbols, sanitizers, unstripped)"
	@printf '%s\n' "  prod_2part: 2-part production build (requires the debug build)"
	@printf '\n'
	@printf '%b\n' '$(BOLD)$(BLUE)Other useful targets::$(RESET)'
	@printf '%s\n' "  covg      : generate coverage via gcov+lcov (this Makefile forces GCC for covg)"
	@printf '%s\n' "  sa        : static analysis (clang-tidy, flawfinder, cppcheck)"
	@printf '%s\n' "  valgrind  : run valgrind (after debug build)"
	@printf '%s\n' "  clean     : clean outputs"
	@printf '%s\n' "  install   : install the built binary to \\$(DESTDIR)\\$(bindir) (use DESTDIR=... prefix=...)"
	@printf '%s\n' "  uninstall : remove installed binary from \\$(DESTDIR)\\$(bindir)"
	@printf '\n'
