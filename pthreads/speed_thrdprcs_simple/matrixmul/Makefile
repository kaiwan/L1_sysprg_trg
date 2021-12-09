# Makefile
ALL := prcs_matrixmul thrd_matrixmul

all: ${ALL}
prcs_matrixmul: prcs_matrixmul.c
	gcc prcs_matrixmul.c -o prcs_matrixmul -Wall
thrd_matrixmul: thrd_matrixmul.c
	gcc thrd_matrixmul.c -o thrd_matrixmul -Wall -pthread
	gcc thrd_matrixmul.c -o thrd_matrixmul_dbg -g -ggdb -O0 -Wall -pthread
clean:
	rm -fv ${ALL}
