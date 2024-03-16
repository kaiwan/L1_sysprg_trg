/*
 * rdwr_fileio.c
 * 
 * Strategy: simple: copy src to dest via the usual read()/write() syscalls
 *
 * History:
 * Author(s) : Kaiwan Billimoria <kaiwan -at- kaiwantech -dot- com>
 * License(s): MIT
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "restart_lib.h"
#include "../../../convenient.h"

/*---------------- Macros -------------------------------------------*/

/*---------------- Typedef's, constants, etc ------------------------*/

/*---------------- Functions ----------------------------------------*/

int main(int argc, char **argv)
{
	int fd_from, fd_to; //, pgsz = getpagesize();
	off_t len;
	struct stat sstat;
	//void *data_src, *origptr, *data_dest;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s source-file dest-file\n", argv[0]);
		exit(1);
	}

	fd_from = r_open2(argv[1], O_RDONLY);
	if (-1 == fd_from) {
		perror("open: src file");
		exit(1);
	}

	fd_to = r_open3(argv[2], O_RDWR | O_CREAT,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (-1 == fd_to) {
		perror("open: dest file");
		exit(1);
	}

	// Query src file size
	if (stat(argv[1], &sstat) == -1) {
		perror("stat");
		exit(1);
	}
	len = sstat.st_size;
	if (len == 0) {
		fprintf(stderr,
			"%s: source-file %s size is zero bytes, aborting...\n",
			argv[0], argv[1]);
		exit(1);
	}

/*
	// Set the dest file to be as large as the source file..
	lseek (fd_to, len-1, SEEK_SET);
	if (write (fd_to, "", 1) < 0) {
		perror("write: to fd_to for 1 byte at eof failed");
		exit(1);
	}
*/
	ssize_t nr, nw;
	size_t RDWR_SIZE = 1024*1024;
	void *buf = malloc(RDWR_SIZE);

	if (!buf) {
		fprintf(stderr, "WARNING: write: not same as read bytes\n");
		exit(1);
	}

	while ((nr = r_read(fd_from, buf, RDWR_SIZE))) {
		nw = r_write(fd_to, buf, nr);
		if (nw == -1) {
			perror("write failed");
			exit(1);
		}
		if (nw != nr)
			fprintf(stderr, "WARNING: write: not same as read bytes\n");
	}

	if (close(fd_from) == -1)
		perror ("close/3 failed");
	if (close(fd_to) == -1)
		perror ("close/3 failed");
	free(buf);
#if 0
	/*
	   void *mmap(void *addr, size_t length, int prot, int flags,
	   int fd, off_t offset);
	 */
	origptr = data_src = mmap(0, len, PROT_READ, MAP_PRIVATE, fd_from, 0);
	if (data_src == (void *)-1) {
		perror("mmap: to src file");
		exit(1);
	}
	MSG("len=%ld data_src = %p\n", len, data_src);

	// Set up a shared file mapping as we want in-memory mods to propogate to the underlying file.
	data_dest = mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd_to, 0);
	if (data_dest == (void *)-1) {
		perror("mmap: to dest file");
		exit(1);
	}

	memcpy (data_dest, data_src, len);
	//pause();

	if (munmap(origptr, len) == -1) {
		perror("munmap");
		exit(1);
	}
	if (close(fd_from) == -1)
		perror ("close/3 failed");

	munmap(data_dest, len);
	if (close(fd_to) == -1)
		perror ("close/4 failed");
#endif
	exit(0);
}

/* vi: ts=4 */
