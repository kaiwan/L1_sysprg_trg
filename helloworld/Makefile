ALL := hello hello_dbg
all: ${ALL}

CC := ${CROSS_COMPILE}gcc
OUT := $(ALL)
ifeq ($(ARCH),arm)
  @echo "Building for ARM"
endif

hello: helloworld.c
	${CC} helloworld.c -o hello -Wall -O2
hello_dbg: helloworld.c
	${CC} -g -ggdb -O0 helloworld.c -o hello_dbg -Wall
clean:
	rm -f ${OUT}
