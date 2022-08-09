# Makefile
# ref/credit: http://nuclear.mutantstargoat.com/articles/make/
csrc := $(wildcard src/*.c) \
        $(wildcard src/engine/*.c) \
        $(wildcard src/audio/*.c)

# NOT working??
#obj := $(csrc:.c=.o)
# Let's explicitly specify the obj's
obj := main.o s2.o engine/s3.o audio/s4.o

all: myapp
CC=${CROSS_COMPILE}gcc
CFLAGS_DBG=-g -ggdb -gdwarf -O0 -Wall -Wextra
CFLAGS=-Wall
LDFLAGS = -lm -lz  #-lGL -lglut -lpng

myapp: $(obj)
	@echo "obj = $(obj), @ = $@, ^ = $^"
	@echo "build rule: \
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) myapp
