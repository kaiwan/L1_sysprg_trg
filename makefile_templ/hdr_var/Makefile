# Makefile
# Also performs dependency tracking (for headers etc)
# ref/credit: http://nuclear.mutantstargoat.com/articles/make/

#---NOTE: keep this UPDATED---------------------------
csrc = $(wildcard *.c) \
        $(wildcard engine/*.c) \
        $(wildcard audio/*.c)
#-----------------------------------------------------
obj = $(csrc:.c=.o)
dep = $(obj:.o=.d)  # one dependency file for each source

# target name is always 'myapp'??
all: myapp
CC=${CROSS_COMPILE}gcc
CFLAGS = -Os -Wall -MMD
CFLAGS_DBG=-g -ggdb -gdwarf -Og -Wall -Wextra -MMD
  # -MMD: option to generate a .d file during compilation
LDFLAGS = #-lm -lrt -lz -lGL -lglut -lpng

myapp: $(obj)
	@echo "obj = $(obj), @ = $@, ^ = $^"
	@echo "build rule: \
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

-include $(dep)   # include all dep files in the makefile

.PHONY: clean
clean:
	rm -f $(obj) *.d myapp
