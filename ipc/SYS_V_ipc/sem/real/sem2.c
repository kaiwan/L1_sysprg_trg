/* 

sem2.c

Demo POSIX:XSI (originally SysV IPC) semaphore usage.
(More) realistic version of (simplistic) sem1 program.

1. Create (or fetch) the binary semaphore
2. Initialize semaphore
3. Lock the semaphore
4. Open the file (shared resource)
5. Processing on the file is done
6. Unlock the resource

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
#include <signal.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sem_lib.h"

#define SEM_PERMS	(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define MAX_TRIES	10
#define FILE_PERMS	SEM_PERMS
#define MY_KEY		0x123654L
#define	SLEEP_TIME	10	// sleep for n seconds..

#define APPNAME	"Sem1"
#define INFO(string, args...) \
	fprintf(stderr, "%s:[%ld]: " string, APPNAME, (long)getpid(), ##args)

int initflag;

/* printerror fn - from "UNIX Systems Programming", Robbins & Robbins. 
   Slightly modified.
*/
inline void printerror (char *prg, char *msg, int error)
{
	fprintf (stderr, "%s:[%ld]:%s:%s\n",
		prg, (long)getpid(), msg, strerror (error));
}

/*--------------------------------------------------------
This (below) is the legacy way of setting up the sembuf ops.
We now do this in an implementation-independent manner using a 
convenience routine - setsembuf.

static struct sembuf	semop_lock[2] = {
	// sem_num, sem_op, flag 
	{0, 0, 0},		  // wait for sem#0 to become 0
	{0, 1, SEM_UNDO}  // then increment sem#0 by 1
};

static struct sembuf	semop_unlock[1] = {
	// sem_num, sem_op, flag 
	{0, -1, (IPC_NOWAIT | SEM_UNDO)}// decrement sem#0 by 1 (sets it to 0)
};
----------------------------------------------------------*/


void sigfunc (int signum)
{
	/* actually should not use [f]printf in sig handler - not aysnc-signal safe */
	INFO ("**Signal %d interruption!**\n", signum);
}

int main( int argc, char *argv[] )
{
	int id = -1, fd=-1, i, n=0;
	struct sigaction act;
	FILE *	fp;
	char	buf[255], dtbuf[80];
	char 	buf2[] = "This is the sample record written to end of file by Sem1";

	struct sembuf semlock[2], semunlock;
	struct semid_ds seminfo;
	/* From include/bits/sem.h :
	 * --snip--
	 * The user should define a union like the following to use it for arguments
	   for `semctl'.

	   union semun
 	  {
	     int val;				<= value for SETVAL
	     struct semid_ds *buf;		<= buffer for IPC_STAT & IPC_SET
	     unsigned short int *array;		<= array for GETALL & SETALL
	     struct seminfo *__buf;		<= buffer for IPC_INFO
	   };

	   Previous versions of this file used to define this union but this is
	   incorrect.  One can test the macro _SEM_SEMUN_UNDEFINED to see whether
	   one must define the union or not.  
	 * --snip--
	 */
       union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux specific) */
       } arg;

	if( argc != 2 ) {
		fprintf( stderr, "Usage: %s file_to_[un]lock\n\
\n***NOTE*** The file will be written to.\n", argv[0] );
		exit( 2 );
	}

	/* Setup signal handler */
	act.sa_handler = sigfunc;
	sigemptyset (&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if (sigaction (SIGINT, &act, 0) ||
		sigaction (SIGQUIT, &act, 0) == -1) {
		printerror(argv[0], "sigaction failure", errno);
		exit(1);
	}

	/* Sem initialization has a fatal flaw in SysV IPC design.
	 * There are two separate calls - semget and semctl (w/ SETVAL) - 
	 * required to create and initailize a semaphore. This gives rise to a
	 * race condition (as the kernel can switch context in between the two
	 * calls, i.e. it's not atomic).
	 *
	 * There is a way to overcome this: basically, we know that sem_otime
	 * member of the semid_ds structure is set to zero when a new semaphore
	 * is created; and set to non-zero (cur time) only by a successful call
	 * to semop. Therefore, we have the second (or third...) process wait 
	 * until sem_otime is non-zero; this implies that the process that 
	 * initialized the semaphore has successfully called semop (which it 
	 * must do).
	 *
	 * See "UNIX Network Programming, IPC" Richard Stevens, 2nd Ed pg 284 
	 * for details.
	 */ 
	/* 1. Create / Get semaphore and initialize it. */
	if (initflag == 0) {
		if ((id = semget (MY_KEY, 1, IPC_CREAT|IPC_EXCL|SEM_PERMS)) >= 0) {
			/* success, first to initialize */
			/* int initelement (int semid, int semnum, int semvalue) */
			if (initelement (id, 0, 1) == -1) {
				printerror (argv[0], 
				"initelement (semctl) semaphore init failure", errno);
			}
			INFO("This process [%d] first to initialize sem...\n", getpid());
		} else if (errno == EEXIST) {
			/* already created; make sure it's initialized.. */
			id = semget (MY_KEY, 1, SEM_PERMS);
			arg.val = 1;
			/* IPC_STAT : Copy  information  from  the  kernel  
			 * data  structure  associated  with semid into the semid_ds 
			 * structure pointed to by arg.buf.  The argument semnum is 
			 * ignored.  The  calling process must have read permission 
			 * on the semaphore set.
			 */
			if (semctl (id, 0, IPC_STAT, &arg) == -1) {
				printerror (argv[0], 
				"semctl semaphore init failure", errno);
			}
			INFO("Attempting initialization loop now...\n");
			arg.buf = &seminfo;
			for (i=0; i<MAX_TRIES; i++) {
				if (semctl (id, 0, IPC_STAT, arg) == -1) {
					printerror (argv[0], 
					"in loop:semctl semaphore init failure", errno);
				}
				if (arg.buf->sem_otime != 0)
					goto init;
				sleep (1);
			}
			printerror (argv[0], "semget succeeded but sem not initialized", 
				errno);
			exit (1);
		} else {
			printerror (argv[0], "semget failed.", errno);
			exit (1);
		}
init:
		initflag = 1;

		






#if 0
	if ((id = semget( MY_KEY, 0, 0 )) == -1 ) {
		/* Not existing, create it */
		if ( ((id = semget (MY_KEY, 1, IPC_CREAT|IPC_EXCL|SEM_PERMS)) == -1) && 
			errno == EEXIST) {
			if ((id = semget( MY_KEY, 0, 0 )) == -1 ) {
				printerror (argv[0], "semget failure 1", errno);
				exit (1);
			}
			else {
				printerror (argv[0], "semget failure 2", errno);
				exit (1);
		}
	  }
	}
#endif
	INFO ("Fetched (and initialized) semaphore ID (for key 0x%x) is %d\n", 
		(unsigned int)MY_KEY, id );

#if 0
	/* int initelement (int semid, int semnum, int semvalue) */
	if (initelement (id, 0, 1) == -1) {
		printerror (argv[0], "initelement (semctl) semaphore init failure", errno);
		if (removesem(id) == -1)
			printerror (argv[0], 
				"removesem (semctl) failed semaphore deletion failure", errno);
		exit (1);
	}
	INFO ("Semaphore initialized...\n");
#endif

	/*
	 * 2. Inititalize the sembuf op's and the semaphore
	 */
	/* void setsembuf (struct sembuf *s, int num, int op, int flg) */
	setsembuf (&semlock[0], 0, 0, 0);	/* setting for locking semaphore, op 0 */
	setsembuf (&semlock[1], 0, 1, SEM_UNDO);/* setting for locking semaphore, op 1 */
	setsembuf (&semunlock, 0, -1, IPC_NOWAIT|SEM_UNDO);/* setting for unlocking 
						semaphore */
	
	/* 
	 * 3. Lock the file (shared resource)
	 * Performs the operation atomically.
	 */
	INFO( "(Attempting to) Lock resource (file %s) now...\n", argv[1] );
	if( r_semop (id, &semlock[0], 2) < 0 ) {
		printerror (argv[0], "semaphore locking failure", errno);
		exit (1);
	}
	INFO("File locked..\n");
   }   

	//-------------------------------Resource Locked.

	/* ----------------- 4. Open the file (shared resource)
	 * Notice the O_APPEND flag, guaranteeing the open and seek to EOF
	 * is carried out as an atomic operation.
	 * (Conversely, performing the open without the O_APPEND flag
	 * and then seeking to EOF is a non-atomic operation with the 
	 * possibility of a context switch.
	 */
	INFO( "File processing is taking place now..\n");
	if( (fd = open( argv[1], O_RDWR|O_APPEND|O_CREAT, FILE_PERMS )) == -1 ) {
		printerror (argv[0], "File open failure", errno);
		exit( 1 );
	}

	/* 5. Process the file.. */

	/* Construct the record buffer to be written */
	/* Get the output of the date command */
	buf[0] = dtbuf[0] = '\0';
	if ((fp = popen( "date", "r" )) == NULL)
		printerror (argv[0], "popen failure", errno);
	if (fgets( dtbuf, 80, fp ) == NULL)
		printerror (argv[0], "fgets failure", errno);
	if (pclose( fp ) == -1)
		printerror (argv[0], "pclose failure", errno);
	snprintf (buf, 128, "\n%s:[%ld]: %s: %s.%ld", 
		APPNAME, (long)getpid(), dtbuf, buf2, (long)getpid());

	if( (n = write( fd, buf, strlen(buf)) ) == -1 ) {
		printerror (argv[0], "File write failed, aborting..", errno);
		close( fd );		
		if( r_semop( id, &semunlock, 1 ) < 0 ) {
			printerror (argv[0], "semaphore unlocking failure", errno);
			exit (1);
		}
		exit( 1 );
	}

	/* Simulate some more processing time by sleeping... */
	//sleep( SLEEP_TIME ); /* TODO- make this r_sleep */
	r_sleep( SLEEP_TIME );

	close( fd );
	INFO("Processing done - Write of %d bytes\n", n );

	/* ------------------ 6. Unlock the resource */
	if( r_semop( id, &semunlock, 1 ) < 0 ) {
		printerror (argv[0], "semaphore unlocking failure", errno);
		exit (1);
	}
	//-------------------------------Resource Unlocked.
	INFO( "File (semaphore) unlocked. Exiting..\n" );
	exit( 0 );
}

/* end sem2.c */

