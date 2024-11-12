/* sfingr.c
* A multiprocess concurrent streams socket (TCP/IP) server.
*  Internet domain streams socket (TCP/IP) server; concurrent server
*
*  Compile as:
*  $ cc sfingr.c -o sfingr -Wall               -no debugging
*  $ cc sfingr.c -o sfingr -Wall -DDBG -g      -with debugging
*/
#include "fingr.h"

#define	QLENGTH		5
#define DBG_MUST_SLEEP  0
#define	DEBUG_SLEEP	20
#define	ZOMBIES		0

static int verbose=0;

// Prevent zombies..
static void sig_child( int signum )
{
#if 0 // we're using SA_NOCLDWAIT to get rid of zombies... no need
	  // to do anything here!
	int status;
	int pid;

	while( (pid=wait3( &status, WNOHANG, 0 )) > 0 ) {
		if( verbose )
			printf("parent server in SIGCHLD handler; pid=%d\n",pid);
	}
#else
	printf("%s()\n", __func__);
#endif
} // sig_child()

static int err_exit( char *prg, char * err, int exitcode )
{
	char *err_str;

	err_str = (char *)malloc(300);
	if( !err_str ) {
		printf("\n%s: malloc failed for err_str..\n", prg);
		printf("%s exiting with error code %d, perror shows:\n", prg, exitcode);
		perror( prg );
		printf("Original error message: %s\n", err);
		exit( exitcode );
	}

	snprintf(err_str, 300, "%s: %s", prg, err);
	perror( err_str );
	free( err_str );
	
	exit( exitcode );
} // err_exit()

// Child server processes this function
static int process_client( int sd, char *prg )
{
	char *username, *reply;
	ssize_t n;
	struct passwd *p;

	MSG ("_svr: in process_client()..\n");
	username = (char *)malloc( 32 );
	if( !username ) {
		printf("\n%s: no memory for username buffer", prg );
		exit( 1 );
	}

	// Read username from client
	if( (n=read(sd,username,MAXBUF)) == -1 )
		err_exit( prg, "socket read error", 6 );
	username[n]='\0';

	reply = (char *)malloc( MAXBUF );
	if( !reply ) {
		printf("\n%s: no memory for reply buffer", prg );
		free (username);
		exit( 1 );
	}

	p = getpwnam( username );
	if( p==NULL ) {
		snprintf(reply, 256+256, "\n%s is unable to get info for %s\n", prg, username );
		if( (write( sd, reply, strlen(reply) )) == -1 )
			err_exit( prg, "socket write error", 6 );
		close( sd );
		free(reply);
		free(username);
		exit( 1 );
	}

	// Got the info; send it to the client..
	snprintf( reply, RE_MAXLEN, "\tUsername: %s\n\t(encrypted) password: %s\n\
\tUID: %d\n\tGID: %d\n\tComment/FullName: %s\n\tHome dir: %s\n\tShell: %s\n",
p->pw_name, p->pw_passwd, p->pw_uid, p->pw_gid, p->pw_gecos, p->pw_dir,
p->pw_shell );
	MSG ("_svr: username is %s, reply is:\n%s\n",username,reply);

	if( (write( sd, reply, strlen(reply) )) == -1 ) {
		free( username );
		free( reply );
		err_exit( prg, "socket write error", 6 );
	}

	free( username );
	free( reply );
	close( sd );

	return(0);
} // process_client()


int main( int argc, char *argv[] )
{
	int sd, newsd;
	socklen_t clilen;
	struct sockaddr_in svr_addr, cli_addr;
	struct sigaction act;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s server-ip-addr [-v]\n", argv[0]);
		exit(1);
	}
	if ((argc==2) && (strcmp(argv[1],"-v"))==0) {
		fprintf(stderr, "Usage: %s server-ip-addr [-v]\n", argv[0]);
		exit(1);
	}

	if ((argc==3) && (strcmp(argv[2],"-v"))==0)
		verbose=1;

	if( (sd = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
		err_exit( argv[0], "socket creation error", 1 );
	if( verbose) printf("%s: tcp socket created\n",argv[0]);

	// Initialize server's address - IP addr + port # - & bind it
	// Required to be in Network-Byte-Order
	svr_addr.sin_family = AF_INET;
	// Have to convert the dotted-decimal str passed to a 32-bit IPv4 addr
	// done w/ inet_addr()
	svr_addr.sin_addr.s_addr = inet_addr( argv[1] );
	// Can use any valid IPaddr on an interface by specifying it as INADDR_ANY
	// INADDR_ANY = 0.0.0.0 => any valid IPaddr
//	svr_addr.sin_addr.s_addr = INADDR_ANY;
	svr_addr.sin_port = htons( SERV_PORT );

	if( bind(sd, (struct sockaddr *)&svr_addr,\
	      sizeof(svr_addr) ) == -1 )
		err_exit( argv[0], "socket bind error", 2 );
	if( verbose) printf("%s: bind done at IP %s port %d\n",argv[0],argv[1],
 	 SERV_PORT);

	// setup the 'backlog', the max Q length wrt the clients
	if( listen(sd, QLENGTH) == -1 )
		err_exit( argv[0], "socket listen error", 3 );
	if( verbose) printf("%s: listen q set up\n",argv[0]);

#if(ZOMBIES==0)
	act.sa_handler=sig_child;
	sigemptyset(&act.sa_mask);
	/* On 2.6 Linux, using the SA_NOCLDWAIT flag makes it easy- zombies are prevented.
     * See man sigaction for details...
     */
        act.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_NOCLDWAIT;
	if( sigaction( SIGCHLD, &act, 0 ) == -1 )
		err_exit( argv[0], "sigaction error", 4 );
	if( verbose) printf("%s: SIGCHLD handler set up\n",argv[0]);
#endif

	while( 1 ) {
		if( verbose) printf("%s: pid %d blocking on accept()..\n",
				argv[0],getpid() );
		fflush( stdout );

		clilen = sizeof( cli_addr );
		if( (newsd = accept(sd, (struct sockaddr *)&cli_addr,\
		      &clilen)) == -1 )	// server blocks here..
				err_exit( argv[0], "socket accept error", 5 );

		printf( "\nServer %s: client connected now from IP %s \
port # %d\n", argv[0], inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port) );
		fflush( stdout );

		// Client connected; process..
		switch( fork() ) {
		  case -1:  perror( "fork error" );
			  fflush(stdout);
			  break;

		  case 0 :  // Child server
			  /* for debugging with gdb/ddd etc, keep it asleep
			   * until user can have the debugger can attach to it..
			   */
			  MSG ("debug %s: pid is %d\n",argv[0],getpid());
			#if( DBG_MUST_SLEEP == 1)
			  MSG ("sleeping now for %ds...\n", DEBUG_SLEEP);
			  sleep(DEBUG_SLEEP);
			#endif

			  close( sd );
			  if( verbose ) {
		 	    printf("%s: child server created; pid %d\n",argv[0],getpid());
			    fflush(stdout);
			  }

			  process_client( newsd, argv[0] );

			  if( verbose ) {
			    printf("%s: child server %d exiting..\n",argv[0],getpid());
			    fflush(stdout);
			  }
			  exit( 0 );

		  default:  // Parent server
		   	  close( newsd );
		} // switch
	} // while

} // main()

// end sfingr.c

