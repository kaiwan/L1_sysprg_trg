ALL := helloworld helloworld_pause helloworld_pause_dbg
all: ${ALL}

CC := ${CROSS_COMPILE}gcc
OUT := $(ALL) trccmd_trc.dat /tmp/trc.txt
ifeq ($(ARCH),arm)
  @echo "Building for ARM"
endif

helloworld: helloworld.c
	${CC} helloworld.c -o helloworld -Wall -O2
	strip --strip-all ./helloworld

helloworld_pause: helloworld_pause.c
	${CC} helloworld_pause.c -o helloworld_pause -Wall -O2
	strip --strip-all ./helloworld_pause
helloworld_pause_dbg: helloworld_pause.c
	${CC} -g -ggdb -O0 helloworld_pause.c -o helloworld_pause_dbg -Wall

clean:
	rm -f ${OUT}
