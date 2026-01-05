/*
 * orphan.c
 * Small fork() demo program to generate an orphan;
 * Run this in the background and look up the PPID field with "ps -l"
 *
 * Author: Kaiwan N Billimoria <kaiwan@kaiwantech.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	pid_t p;

	p = fork();
	switch (p) {		/* often coded as switch( (p=fork()) ) */
	case -1:
		perror("fork failed"), exit(1);
	case 0:		// Child
		printf("Child pid %d : p=%d; sleeping now for 10s..\n",
		       getpid(), p);
		sleep(10);
		printf("Child: sleep done, exiting.\n");
		exit(0);
	default:		// Parent
		printf("Parent pid %d : p=%d; exiting now \
(resulting in an orphaned child)..\n", getpid(), p);
		exit(0);
	}
}
