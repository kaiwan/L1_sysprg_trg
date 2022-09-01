# Makefile
# A 'template' for a very simple Makefile.
#
# We highly reccommend that before using it, you look closely at our much
# superior 'better Makefile' template here:
#  0_Makefile_template/killer
ALL = assert_try assert_try2
all: ${ALL}

CFLAGS = -Wall -Os
CFLAGS_DBG = -Wall -Wextra -O0 -g

assert_try: assert_try.c
	gcc ${CFLAGS} assert_try.c -o assert_try
assert_try_dbg: assert_try.c
	gcc ${CFLAGS_DBG} assert_try.c -o assert_try_dbg

assert_try2: assert_try2.c
	gcc ${CFLAGS} assert_try2.c -o assert_try2
assert_try2_dbg: assert_try2.c
	gcc ${CFLAGS_DBG} assert_try2.c -o assert_try2_dbg

clean:
	rm -f ${ALL}

