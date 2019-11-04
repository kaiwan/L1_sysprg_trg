/*
* hdr_shm.h
*
* Common header for simple shmem demo apps, shm1_w.c and shm1_r.c .
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>	// for PATH_MAX

#define PROJECT_ID	0

#define SHMSZ_OUR_MAX	(100*1024*1024)	// 100 MB
//#define SHM_SZ		512	// in bytes
#define MODE		0600
#define LINELENGTH	132
#define NUM		 10

/*
 * In a production environment, we may not be able to hardcode the 'pathname'
 * value for ftok() : hence, we assume a master script (or something) has set
 * the pathname in an environment variable PROJM_SHM_PATH.
 */
key_t get_key()
{
	char *pathmem=NULL, *path=NULL;
	key_t key=0L;

	/* App logic: the pathname for ftok() is stored in the environment 
	 * variable PROJM_SHM_PATH . Look it up...
	 */
	if( (pathmem=malloc(PATH_MAX)) == NULL ) {
		fprintf(stderr,"get_key() failure: malloc failed\n");
		exit(1);
	}
	path=pathmem;
	path=getenv("PROJM_SHM_PATH");
	if( path == NULL ) {
		fprintf(stderr,"Set environment variable PROJM_SHM_PATH to \
pathname of shm path file..\n");
		free (pathmem);
		exit(1);
	}
	
	key = ftok (path, PROJECT_ID);
	free (pathmem);
	return key;
}

