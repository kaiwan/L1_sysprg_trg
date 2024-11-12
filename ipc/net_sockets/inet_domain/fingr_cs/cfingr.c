/* cfingr.c
*  Internet domain streams socket client
*  Usage: cfingr username
*/
#include "fingr.h"
int err_exit( char *prg, char * err, int exitcode );

#define TESTING 0

int main( int argc, char *argv[] )
{
	int sd, n;
	struct sockaddr_in svr_addr;
	char *reply;

	if( argc != 3 )
		fprintf(stderr,"Usage: %s serv-ip-address username\n",argv[0]), exit( 1 );

	if( (sd = socket( AF_INET, SOCK_STREAM, 0 )) == -1 )
		err_exit( argv[0], "socket creation error", 1 );

	// Setup the sockaddr for connecting to the server.
	// Required to be in Network-Byte-Order
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = inet_addr( argv[1] );  // IP addr
	svr_addr.sin_port = htons( SERV_PORT );		// port #

	// Try connecting to the server..
	if( (connect(sd, (struct sockaddr *)&svr_addr, sizeof(svr_addr))) == -1 )
		err_exit( argv[0], "connection error", 2 );

	reply = (char *)malloc(MAXBUF);
	if( !reply ) {
		printf("\n%s: malloc failed for reply..",argv[0]);
		close(sd);
		exit( 7 );
	}

	// Client request (username) is in argv[1]
	printf("%s: sending request \"%s\" to server\n",argv[0],argv[2]);
	if( ((n=write(sd,argv[2],strlen(argv[2]))) == -1 ))
		err_exit( argv[0], "socket write error", 3 );

	if( ((n=read(sd,reply,MAXBUF)) == -1 ))
		err_exit( argv[0], "socket read error", 4 );

	printf("\n%s: Received %d bytes from server;\n%s", argv[0], \
n, reply);

#if(TESTING==1)
	sleep(3);
#endif

	free(reply);
	close( sd );
	exit( 0 );
} // main()


int err_exit( char *prg, char * err, int exitcode )
{
	char *err_str;

	err_str = (char *)malloc(strlen(prg)+strlen(err)+2);
	if( !err_str ) {
		printf("\n%s: malloc failed for err_str..",prg);
		printf("%s exiting with error code %d, perror shows:\n", prg, exitcode);
		perror( prg );
		exit( exitcode );
	}
	
	sprintf(err_str,"%s: %s", prg, err);
	perror( err_str );
	free( err_str );
	
	exit( exitcode );
} // err_exit()

// end cfingr.c
