/* 

sem1.c

1. Create (or fetch) the binary semaphore
2. Lock the semaphore
3. Open the file (shared resource)
4. Processing on the file is done
5. Unlock the resource

SEM_UNDO is used to have the kernel adjust the semaphore 
value on process (abnormal) exit.

*/
//extern int errno;
#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MY_KEY		123654L
#define	SLEEP_TIME	5	// sleep for n seconds..

static struct sembuf	semop_lock[2] = {
	// sem_num, sem_op, flag 
	{0, 0, 0},		  // wait for sem#0 to become 0
	{0, 1, SEM_UNDO}  // then increment sem#0 by 1
};

static struct sembuf	semop_unlock[1] = {
	// sem_num, sem_op, flag 
	{0, -1, (IPC_NOWAIT | SEM_UNDO)}	// decrement sem#0 by 1 (sets it to 0)
};

int id = -1;

int main( int argc, char *argv[] )
{
	int	fd=-1, n=0;
	FILE *	fp;
	char	buf[255], dtbuf[80];
	char 	buf2[] = "This is the sample record written to end of file by sem1.";

	if( argc != 2 ) {
		fprintf( stderr, "Usage: %s filename\n", argv[0] );
		exit( 2 );
	}

#if 0
	if( access( argv[1], F_OK ) == -1 ) {
		perror( "File existence check" );
		exit( 1 );
	}
#endif
	// 1. Create (or fetch if existing) the binary semaphore
	id = semget( MY_KEY, 1, IPC_CREAT | IPC_EXCL | 0666 );
	if( id == -1 ) {
		perror( "Semaphore create" );
		if( errno != EEXIST ) {
			perror("semget failure");
			exit(1);
		}
		id = semget( MY_KEY, 1, 0 );
		if( id == -1 ) 
			perror( "Semaphore create" ),exit(1);
	}
	printf( "\nSem1: Fetched semaphore..id = %d\n", id );

	// 2. Lock the file (shared resource)
	// Performs two operations atomically.
	printf( "\nSem1: Locking resource (file %s)...", argv[1] );

	if( semop( id, &semop_lock[0], 2 ) < 0 )
		perror( "sem lock error" ), exit( 1 );
	printf("\nSem1: File locked.." );

	/* ----------------- 3. Open the file (shared resource)
	 * Notice the O_APPEND flag, guaranteeing the open and seek to EOF
	 * is carried out as an atomic operation.
	 * (Conversely, performing the open without the O_APPEND flag
	 * and then seeking to EOF is a non-atomic operation with the 
	 * possibility of a context switch.
	 */
	printf( "\nSem1: File processing is taking place now.." );
	fflush( stdout );
	if( (fd = open( argv[1], O_RDWR|O_CREAT|O_APPEND, 0644 )) == -1 ) {
		perror( "File open failure" );
		exit( 1 );
	}

	// 4. Process the file..

	// Get the output of the date command
	fp = popen( "date", "r" );
	if (!fgets( dtbuf, 80, fp )) {
		fprintf(stderr, "%s: fgets failed!\n", argv[0]);
		semop( id, &semop_unlock[0], 2 );
		pclose( fp );
		close (fd);
		exit (1);
	}
	pclose( fp );
	sprintf( buf, "\n---\nSem1: PID %d: %s: %s\n", getpid(), dtbuf, buf2 ); 

	if( (n = write( fd, buf, strlen(buf)) ) == -1 ) {
		perror( "File write failed" );
		close( fd );		
		if( semop( id, &semop_unlock[0], 1 ) < 0 )	
			perror( "sem unlock error" ), exit( 1 );
		exit( 1 );
	}

	// Simulate some more processing time by sleeping...
	sleep( SLEEP_TIME );
	close( fd );
	printf("\nSem1: Processing done - Write of %d bytes\n", n );

	// ------------------ 5. Unlock the resource
	if( semop( id, &semop_unlock[0], 1 ) < 0 )	
		perror( "sem unlock error" ), exit( 1 );
	printf( "\nSem1: File (semaphore) unlocked\nSem1 exiting.." );

	exit( 0 );
}

// end sem1.c

