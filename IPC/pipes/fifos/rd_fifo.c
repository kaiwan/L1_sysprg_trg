/*
 * rd_fifo.c
 * Ref / Src : http://stackoverflow.com/questions/2784500/how-to-send-a-simple-string-between-two-programs-using-pipes
 *
 * Error checking, minor enhancements; Kaiwan NB.
 * IMP: See fifo(7) man page for theory of operation.
 *
 * man 3 mkfifo
 * " ... Once  you have created a FIFO special file in this way, any process 
    can open it for reading or writing, in the same way as an ordinary file.
    However, it has to be open at both ends simultaneously before you can proceed 
	to do any input or output operations on it.   Opening  a  FIFO
    for  reading normally blocks until some other process opens the same FIFO for 
	writing, and vice versa.  See fifo(7) for nonblocking handling
       of FIFO special files. ..."
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BYTES 1024

int main(int argc, char **argv)
{
    int fd;
    char * myfifo = "./myfifo", buf[MAX_BYTES];

	/* 
	 * Open the FIFO for reading; it will block until also opened for reading
	 * by it's peer!
	 * !NOTE! Here we anticipate that the writer app runs first, thus creating the FIFO 
	 * and blocking on this read open below...
	 */
    fd = open(myfifo, O_RDONLY);
	if (fd == -1) {
	  perror("open failed");
	  exit (1);
	}
    if (read(fd, buf, MAX_BYTES) == -1) {
	  perror("read failed");
	  exit (1);
	}
    close(fd);

	printf("%s: Received data:\n"
	       "%s\n", argv[0], buf);

    return 0;
}
