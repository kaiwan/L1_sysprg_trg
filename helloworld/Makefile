ALL := helloworld helloworld_dbg helloworld_pause helloworld_pause_dbg
all: ${ALL}

CC := ${CROSS_COMPILE}gcc
OBJDUMP := ${CROSS_COMPILE}objdump
OUT := $(ALL) *.disas trccmd_trc.dat /tmp/trc.txt
ifeq ($(ARCH),arm)
  @echo "Building for ARM"
endif

helloworld: helloworld.c
	${CC} helloworld.c -o helloworld -Wall
	#strip --strip-all ./helloworld
	$(OBJDUMP) -d helloworld > helloworld.disas
	ls -l helloworld helloworld.disas
helloworld_dbg: helloworld.c
	${CC} -g helloworld.c -o helloworld_dbg -Wall
	$(OBJDUMP) -d -S --source-comment="< C code, assembly follows below >" helloworld_dbg > helloworld_dbg.disas
	ls -l helloworld_dbg helloworld_dbg.disas

helloworld_pause: helloworld_pause.c
	${CC} helloworld_pause.c -o helloworld_pause -Wall
	#strip --strip-all ./helloworld_pause
	$(OBJDUMP) -d helloworld_pause > helloworld_pause.disas
	ls -l helloworld_pause helloworld_pause.disas
helloworld_pause_dbg: helloworld_pause.c
	${CC} -g helloworld_pause.c -o helloworld_pause_dbg -Wall
	$(OBJDUMP) -d -S  --source-comment="< C code, assembly follows below > " helloworld_pause_dbg > helloworld_pause_dbg.disas
	ls -l helloworld_pause_dbg helloworld_pause_dbg.disas
ftrace:
	make helloworld
	./trace_helloworld.sh
clean:
	rm -f ${OUT}
