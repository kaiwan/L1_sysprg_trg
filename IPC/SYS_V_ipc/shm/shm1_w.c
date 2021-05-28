/*
* shm1_w.c
*
* Simple shared memory demo app.
* Producer: this program creates a shared memory segment, and writes
* some user data into it.
*
* GNU GPL v2.
* kaiwan.
*/

#include "hdr_shm.h"
#define FORK_SHMEM_DEMO		1

int main( int argc, char **argv )
{
	int shm_id;		/* shared memory id */		
	key_t shm_key;		/* key for shared memory access */
	char *shm_addr;		/* virtual address of shared memory*/
	int n;			/* for loop counter */
	int incr;		/* increment for next string*/
	unsigned long shmsz=0;

	if (argc != 2) {
		fprintf(stderr,"Usage: %s shmem-seg-size-KB\n", argv[0]);
		exit(1);
	}
	shmsz = strtoul(argv[1], 0, 0);
	shmsz *= 1024;
	if ((shmsz <= 0) || (shmsz > SHMSZ_OUR_MAX)) {
		fprintf(stderr,"%s: specified shmem seg size [%lu] invalid..\n"
		"[fyi, our max is %d MB]\n", argv[0], shmsz, (SHMSZ_OUR_MAX/(1024*1024)));
		exit(1);
	}

	/* Fetch the key value; we're using ftok() here... */
	shm_key = get_key();
	if( shm_key == -1 ) {
		fprintf(stderr,"%s: ftok() failed, cannot get key value\n",argv[0]);
		exit( 1 );
	}
	printf("%s: key=%d, size=%lu\n", argv[0], shm_key, shmsz);

	/* Create and initialize the shared memory */
	shm_id = shmget(shm_key, shmsz, (IPC_CREAT|IPC_EXCL|MODE));
	if (shm_id < 0 ) {
		if( errno == EEXIST ) {
			printf("%s: shmem segment already exists, will attempt \
to access it...\n",
			 argv[0]); 
			shm_id = shmget( shm_key, 0, 0 );
			if (shm_id < 0 ) {
				perror("shm1: shmget failed"); 
				exit( 1 );
			}
		}
	}
	printf ("%s: shmem segment successfully created / accessed. ID=%d\n",
		argv[0], shm_id);

	/* Attach to the shmem segment.
	 syntax: shmat( id, shmaddr, flag );
	*/
	shm_addr = shmat( shm_id, 0, 0);
	if( shm_addr == (void *)-1) {
		perror( "shmat failed");
		exit(1);
	}
	printf ("%s: Attached successfully to shmem segment at %p\n",
		argv[0], (void *)shm_addr);

	/* Prompt user for strings */
	for (n=0; n < NUM; n++) {
		printf( "%02d: Enter a string : ", n);
		if (fgets(shm_addr,LINELENGTH,stdin) == NULL)
			fprintf(stderr,"%s: !WARNING! fgets() failed.\n",argv[0]);
		shm_addr[strlen(shm_addr)-1]='\0'; // rm trailing \n
		incr = strlen(shm_addr) + 1;
		shm_addr += incr;
	}

#if (FORK_SHMEM_DEMO==1)
	/* If this process forks, the child will inherit the shmem segment,
 	 * bumping up the attach count 'nattch'. Demo-ed below, if interested..
 	 */

	printf("Before fork: Executing \"ipcs -m\" - see nattch..\n");
	if (system( "ipcs -m" ) < 0) {
		perror( "system ipcs -m failed");
		exit(1);
	}

	// now fork and see ipcs -m in child..is it also attached?
	fflush(stdout);
	switch( fork() ) {
		case -1: perror("fork failed"),exit(1);
		case 0 : // child
			printf("CHILD:: Executing \"ipcs -m\" - see nattch..\n");
			if (system( "ipcs -m" ) < 0) {
				perror( "system ipcs -m failed");
				exit(1);
			}
			sleep(10);
			exit( 0 );

		default: // parent
			wait(0);
			printf("parent %d exiting..\n",getpid());
	}
#endif
	exit( 0 );
}

