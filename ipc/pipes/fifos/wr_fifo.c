/*
 * wr_fifo.c
 * Ref / Src : http://stackoverflow.com/questions/2784500/how-to-send-a-simple-string-between-two-programs-using-pipes
 *
 * man 3 mkfifo
 * " ... Once  you have created a FIFO special file in this way, any process 
    can open it for reading or writing, in the same way as an ordinary file.
    However, it has to be open at both ends simultaneously before you can proceed 
	to do any input or output operations on it.   Opening  a  FIFO
    for  reading normally blocks until some other process opens the same FIFO for 
	writing, and vice versa.  See fifo(7) for nonblocking handling
       of FIFO special files. ..."

 * Error checking, minor enhancements; Kaiwan NB.
 * IMP: See fifo(7) man page for theory of operation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd;
    char * myfifo = "./myfifo", *msg="Hi FIFO, wassup";

    /* Create, from scratch, the FIFO (named pipe) */
    unlink(myfifo);
    if (mkfifo(myfifo, 0666) == -1) {
	  perror("mkfifo failed");
	  exit (1);
	}

	/* 
	 * Open the FIFO for writing; it will block until also opened for reading
	 * by it's peer!
	 */
    fd = open(myfifo, O_WRONLY);
	if (fd == -1) {
	  perror("open failed");
      unlink(myfifo);
	  exit (1);
	}
    write(fd, msg, strlen(msg));
    close(fd);

    return 0;
}
