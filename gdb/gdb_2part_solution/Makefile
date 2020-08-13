# Makefile
# for Linux Debugging trg for 1_buggy.c
# 
# For ARM/MIPS/PPC cross compilation, set the env var
# CROSS_COMPILE to the cross compiler prefix when invoking make, eg. for ARM-32:
# make CROSS_COMPILE=arm-none-linux-gnueabi- all
#
# Kaiwan NB, kaiwanTECH.
CC := ${CROSS_COMPILE}gcc
DBG_FLAGS := -g -ggdb -gdwarf-4 -O0 -DDEBUG -Wall
ALL := 1_buggy_sym 1_buggy_rel
all: ${ALL}

1_buggy_sym: 1_buggy.c
	${CC} ${DBG_FLAGS} 1_buggy.c -o 1_buggy_sym
1_buggy_rel: 1_buggy.c
	cp 1_buggy_sym 1_buggy_rel || exit 1
	strip --strip-all ./1_buggy_rel
	#strip --strip-debug ./1_buggy_rel
	objcopy --add-gnu-debuglink=1_buggy_sym ./1_buggy_rel
clean:
	rm -fv *.o ${ALL} core*
