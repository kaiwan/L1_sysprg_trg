/*
 * hxdump_buggy.c
 *
 * Objective:
 * ----------
 * The pathnames to files are provided in a data file (pathname_file.txt).
 * Eg.
 * $ cat pathname_file.txt
 *   /etc/passwd
 *   /sbin/init
 *   /bin/sh
 * $
 *
 * We want to be able to process all the files in that data file. The actual
 * 'processing' of a file involves reading in it's first "num" bytes (passed 
 * as a parameter), and displaying them in hexadecimal to stdout.
 *
 * As of now, it fails with the following output:
 *
 * $ ./hxdump_buggy pathname_file.txt 100
 * open: No such file or directory
 * $ 
 * 
 * 1. Debug and correct this.
 *
 * 2. Once working, the output should look like this:
 * $ ./getfiles pathname_file.txt 100
 * -----------------------------------------------------------------------
 * Reading /etc/passwd...
 * -----------------------------------------------------------------------
 * 61 74 3a 78 3a 32 35 3a 32 35 3a 42 61 74 63 68 20 6a 6f 62
 * 73 20 64 61 65 6d 6f 6e 3a 2f 76 61 72 2f 73 70 6f 6f 6c 2f
 * 61 74 6a 6f 62 73 3a 2f 62 69 6e 2f 62 61 73 68 0a 62 65 61
 * 67 6c 65 69 6e 64 65 78 3a 78 3a 31 30 32 3a 31 30 34 3a 55
 * 73 65 72 20 66 6f 72 20 42 65 61 67 6c 65 20 69 6e 64 65 78
 * ...
 * $
 * 
 * displays the first 100 bytes of /etc/passwd in hexadecimal.
 *
 * 3. Additional Requirement: Display the offset position in the first
 * column and display the ASCII equivalent of the hex values on the 
 * right-hand side (unprintable characters should display as a '.'). 
 * Properly working example:
 *
$ ./hxdump pathname_file.txt 100
-----------------------------------------------------------------------
Reading /etc/passwd...
-----------------------------------------------------------------------
00000000  61 74 3a 78 3a 32 35 3a 32 35           at:x:25:25
00000010  3a 42 61 74 63 68 20 6a 6f 62           :Batch job
00000020  73 20 64 61 65 6d 6f 6e 3a 2f           s daemon:/
00000030  76 61 72 2f 73 70 6f 6f 6c 2f           var/spool/
00000040  61 74 6a 6f 62 73 3a 2f 62 69           atjobs:/bi
00000050  6e 2f 62 61 73 68 0a 62 65 61           n/bash.bea
00000060  67 6c 65 69 6e 64 65 78 3a 78           gleindex:x
00000070  3a 31 30 32 3a 31 30 34 3a 55           :102:104:U
00000080  73 65 72 20 66 6f 72 20 42 65           ser for Be
00000090  61 67 6c 65 20 69 6e 64 65 78           agle index
-----------------------------------------------------------------------
Reading /sbin/init...
-----------------------------------------------------------------------
00000000  7f 45 4c 46 01 01 01 00 00 00           .ELF......
00000010  00 00 00 00 00 00 02 00 03 00           ..........
00000020  01 00 00 00 50 81 04 08 34 00           ....P...4.
00000030  00 00 8c c2 07 00 00 00 00 00           ..........
00000040  34 00 20 00 06 00 28 00 1a 00           4. ...(...
00000050  19 00 01 00 00 00 00 00 00 00           ..........
00000060  00 80 04 08 00 80 04 08 c0 8c           ..........
00000070  07 00 c0 8c 07 00 05 00 00 00           ..........
00000080  00 10 00 00 01 00 00 00 00 90           ..........
00000090  07 00 00 10 0c 08 00 10 0c 08           ..........
-----------------------------------------------------------------------
Reading getfiles...
-----------------------------------------------------------------------
00000000  7f 45 4c 46 01 01 01 00 00 00           .ELF......
<o/p truncated here...>
$ 

 * Get it to work as shown above (you could use 20 bytes per line as well).
 * Note: File(s) are currently read in a single operation (unoptimized); hence,
 * the max # bytes that currently can be read (at a time) is 2^31-1.
 * You can optimize this by putting the reads in a loop..
 */
#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXCOL	20

int main( int argc, char **argv)
{
	FILE *fp;
	char *path, *buf;
	int fd, n, j, len;
	
	if (argc != 3) {
		fprintf (stderr, "Usage: %s pathnames_datafile num\n\
\tnum: number of bytes to display from BOF in hexadecimal\n", argv[0]);
		exit (1);
	}
	len = atoi(argv[2]);
	if (len <=0) {
		fprintf (stderr, "%s: Invalid number of bytes to read : %d\n",
			argv[0], len);
		exit (1);
	}
	
	fp = fopen (argv[1], "r");
	if (!fp) {
		perror ("fopen failed");
		exit (1);
	}
	
	if (!(path = malloc (PATH_MAX))) {
		perror ("malloc");
		fclose (fp);
		exit (1);
	}
	
	/* Process all files in the data file (argv[1]) */
	while (fgets(path, PATH_MAX, fp)) {      /* Retrieve the pathname */
		if ((fd = open (path, O_RDONLY)) == -1) {
			free (path);
			fclose (fp);
			exit (1);
		}

		if (!(buf = malloc (len))) {
			perror ("malloc");
			close (fd);
			free (path);
			fclose (fp);
			exit (1);
		}
		
		printf ("\
-----------------------------------------------------------------------\n\
Reading %s...\n\
-----------------------------------------------------------------------\n",
			path);

		/* Read in 'len' bytes */
		while ((n=read (fd, buf, len))) {
			if (n == -1) {
				perror ("open");
				close (fd);
				free (path);
				fclose (fp);
				exit (1);
			}
			if (n >= len)
				break;
		}
		close (fd);
		
		/* Print in hex */
		for (j=0; j<len; j++) {
			printf ("%02x ", (unsigned char)buf[j]);
			/* TODO: 1. print 20 hex values, leave some whitespace, then print 
			            corresponding 20 ASCII values as well on the right.
			         2. print offsets in left-column.
			 */
			
			if (j && !(j%MAXCOL))
				printf("\n");
		}
		printf("\n");
		free (buf);
	}
	
	free (path);
	if (fclose (fp)) {
		perror ("fclose failed");
		exit (1);
	}
	exit (0);
}

