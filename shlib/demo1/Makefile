# Makefile
# Small demo for shlib
all: mymathlib main
mymathlib: mymathlib.c
	gcc -c mymathlib.c -Wall -Wextra -fPIC -o mymathlib.o
	gcc -shared mymathlib.o -o libmymath.so
	ar rcs libmymathar.a mymathlib.o
main: main.c
	# -lfoo: implies the shlib is named libfoo.so
	gcc main.c -o main -Wall -L. -lmymath
	ldd main
	gcc main.c -L. -Wl,--whole-archive -lmymathar -Wl,--no-whole-archive -o main_static
#                      |____________________________|
#                         Only libmymath is affected
clean:
	-rm main main_static *.a *.o libmymath.so 2>/dev/null
