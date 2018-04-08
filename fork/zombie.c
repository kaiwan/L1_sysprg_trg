/*
 * zombie.c
 * Small fork() demo program to generate a zombie;
 * Run this in the background and look up the status field with "ps -l";
 *  should be "Z".
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
 *
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
		printf
		    ("Child pid %d : p=%d; exiting now (generating a zombie!)..\n",
		     getpid(), p);
		exit(0);
	default:		// Parent
		printf("Parent pid %d : p=%d; sleeping now for 300s without wait()-ing \
(resulting in an orphaned child)..\n", getpid(),
		       p);
		sleep(300);
		printf("Parent: sleep done, exiting. Notice how (on Linux) the \
zombie is now cleared!\n");
		exit(0);
	}
}
