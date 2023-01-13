ALL := helloworld helloworld_dbg
all: ${ALL}

CC := ${CROSS_COMPILE}gcc
OUT := $(ALL)
ifeq ($(ARCH),arm)
  @echo "Building for ARM"
endif

helloworld: helloworld.c
	${CC} helloworld.c -o helloworld -Wall -O2
	strip --strip-all ./helloworld
helloworld_dbg: helloworld.c
	${CC} -g -ggdb -O0 helloworld.c -o helloworld_dbg -Wall
clean:
	rm -f ${OUT}
