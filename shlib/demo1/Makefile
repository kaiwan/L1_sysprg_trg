# Makefile
# Small demo for shlib
all: mymathlib main
mymathlib: mymathlib.c
	gcc -c mymathlib.c -Wall -Wextra -fPIC -o mymathlib.o
	gcc -shared mymathlib.o -o libmymath.so
main: main.c
	# -lfoo: implies the shlib is named libfoo.so
	gcc main.c -o main -Wall -L. -lmymath
	ldd main
clean:
	rm main *.o libmymath.so 2>/dev/null
