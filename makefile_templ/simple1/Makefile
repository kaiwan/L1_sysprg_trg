# Makefile
# ref/credit: http://nuclear.mutantstargoat.com/articles/make/
csrc := $(wildcard *.c) \
        $(wildcard engine/*.c) \
        $(wildcard audio/*.c)
# Convert src to obj's
objs := $(csrc:.c=.o)

ALL := myapp myapp_dbg
all: $(ALL)

CC=${CROSS_COMPILE}gcc
POSIX_STD=201112L
STD_DEFS=-D_DEFAULT_SOURCE -D_GNU_SOURCE

CFLAGS=-Wall -O2 -UDEBUG -D_POSIX_C_SOURCE=${POSIX_STD} ${STD_DEFS} -D_FORTIFY_SOURCE=2
CFLAGS_DBG=-g -ggdb -gdwarf -O0 -Wall -Wextra -fno-omit-frame-pointer -DDEBUG \
		   -D_POSIX_C_SOURCE=${POSIX_STD} ${STD_DEFS} -D_FORTIFY_SOURCE=2
LDFLAGS = -lm -lz  #-lGL -lglut -lpng

# Explanation - Makefile shorthand vars:
#  $@ = the name of the target
#  $^ = the objs to link in
myapp: $(objs)
	@echo "[DBG] csrc = $(csrc), objs = $(objs), @ = $@, ^ = $^"
	@echo "[DBG] build rule: \
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

myapp_dbg: $(objs)
	@echo "[DBG] csrc = $(csrc), objs = $(objs), @ = $@, ^ = $^"
	@echo "[DBG] build rule: \
	$(CC) $(CFLAGS_DBG) -o $@ $^ $(LDFLAGS)"
	$(CC) $(CFLAGS_DBG) -o $@ $^ $(LDFLAGS)

myapp_dbg_asan: $(objs)
	$(CC) $(CFLAGS_DBG) -fsanitize=address -o $@ $^ $(LDFLAGS)
myapp_dbg_ubsan: $(objs)
	$(CC) $(CFLAGS_DBG) -fsanitize=undefined -o $@ $^ $(LDFLAGS)
myapp_dbg_tsan: $(objs)
	$(CC) $(CFLAGS_DBG) -fsanitize=thread -o $@ $^ $(LDFLAGS)
myapp_dbg_lsan: $(objs)
	$(CC) $(CFLAGS_DBG) -fsanitize=leak -o $@ $^ $(LDFLAGS)
# Memory Sanitizer deprecated?
#myapp_dbg_msan: $(objs)
#	$(CC) $(CFLAGS_DBG) -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(objs) $(ALL) myapp_dbg_*
