/* 
 * xfer_file.c
 * 
 * Simple client/server TCP streams socket implementation to transfer a file, from
 * server to client process.
 *
 * The intention being to test and contrast this via two approaches:
 * 1. The "traditional" less sophisticated approach:
 *  looping over: the src file with read(2), writing to the dest file with write(2) syscalls.
 * 2. The "modern" sophisticated way, via the sendfile(2) syscall - the 
 * so-called "zero-copy" approach.
 *
 * By passing appropriate parameters, one can run this program as either server or client;
 * also, when run as a server, one can specify whether to use the 'modern' or 'traditional' approach.
 *
 * Useful res:
 * o "Zero Copy I: User-Mode Perspective" by Dragan Stancevic.
 * http://www.linuxjournal.com/article/6345?page=0,0
 * o "Efficient data transfer through zero copy"
 * http://www.ibm.com/developerworks/linux/library/j-zerocopy/
 *
 * [L]GPL v2.
 * (c) kaiwan.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "restart_lib.h"

//#define	SERV_IP		"172.16.255.141"
#define	SERV_IP		"127.0.0.1"
#define SERV_PORT	7100
#define DESTFILE "./destfile.dat"
#define TRADITIONAL 1
#define MODERN 		2

#define VP(str, args...) \
do { \
	if (verbose) \
		printf(str, ##args); \
} while (0)

static int verbose=0;

// Prevent zombies..
static void sig_child(int signum)
{
	int status;
	int pid;

	while( (pid=wait3( &status, WNOHANG, 0 )) > 0 ) {
			VP("parent server in SIGCHLD handler; pid=%d\n",pid);
	}
} // sig_child()

static inline int err_exit(char *prg, char * err, int exitcode)
{
	char err_str[512];

	snprintf(err_str, 511, "%s: %s", prg, err);
	perror(err_str);
	exit(exitcode);
} // err_exit()

// Server processes this function
static int process_client(int sd, int howto, char *prg)
{
	char *file2send=NULL;
	struct stat sb;
	int fd_send, n=0, tsent=0;

	file2send = (char *)malloc(PATH_MAX);
	if( !file2send ) {
		printf("\n%s: no memory for username buffer", prg );
		exit( 1 );
	}

	// Read file2send from client
	if((n=read (sd, file2send, PATH_MAX)) == -1) {
		free(file2send);
		err_exit( prg, "svr: socket read error", 1);
	}
	file2send[n]='\0';

	VP("svr: to send file \"%s\" to client...\n", file2send);
	if ((fd_send = r_open2(file2send, O_RDONLY)) == -1) {
		free(file2send);
		err_exit(prg, "svr: file open error", 1);
	}

	if (howto == MODERN) {
		VP("svr: using MODERN sendfile(2) approach...\n");
		if (fstat (fd_send, &sb) < 0) {
			free(file2send);
			err_exit(prg, "svr: fstat error", 1);
		}
		//tsent = sendfile64 (sd, fd_send, NULL, sb.st_size);
		tsent = sendfile (sd, fd_send, NULL, sb.st_size);
		if (tsent < 0) {
			free(file2send);
			err_exit(prg, "svr: sendfile error", 1);
		}
	} else if (howto == TRADITIONAL) {
		VP("svr: using TRADITIONAL read/write loop approach...\n");
		// Send file data to client
		while (1) {
			int nr;
			char rbuf[getpagesize()];

			nr = r_read(fd_send, rbuf, sizeof(rbuf));
			if (0 == nr)
				break;
			else if (nr < 0)
				err_exit( prg, "svr: read error", 1);

			if (r_write (sd, rbuf, nr) <= 0)
				err_exit( prg, "svr: write error", 1);
			tsent += nr;
		}
	}
	VP("svr: # bytes sent    : %16d\n", tsent);

	free(file2send);
	r_close(fd_send);
	r_close(sd);

	return(0);
} // process_client()


static inline void usage(char *name)
{
		fprintf(stderr, "\
Usage:\n\
%s -s -{t|m} [-v]                <--- as server\n\
-OR-\n\
%s file-to-send [-v]             <--- as client\n\
\n\
 -s : run the program as the server\n\
  -t : use the _t_raditional 'loop over read/write syscalls' approach\n\
  -m : use the _m_odern 'single sendfile(2) syscall' approach\n\
 If '-s' not present, it implies it's the client and you must pass a valid pathname as file to 'GET' from the server.\n", 
 			name, name);
}

int main(int argc, char *argv[])
{
	int is_server=0, how=0, sd, newsd;
	socklen_t clilen;
	struct sockaddr_in svr_addr, cli_addr;
	struct sigaction act;

	// ---------------Process Arguments
	if (argc < 2) {
		usage(argv[0]);
		exit (1);
	}
	if (argv[1][0] == '-' && argv[1][1] == 's') { // '-s' passed; this is the server
		is_server=1;
		if (argc < 3) {
			usage(argv[0]);
			exit (1);
		}
		if (argv[2][0] == '-' && argv[2][1] == 't') { // '-t': traditional approach
			how = TRADITIONAL;
			VP("svr: will use TRADITIONAL (multiple) read/write loop approach.\n");
		} else if (argv[2][0] == '-' && argv[2][1] == 'm') { // '-m': modern approach
			how = MODERN;
			VP("svr: will use MODERN (single) sendfile(2) approach.\n");
		}
		else {
			usage(argv[0]);
			exit (1);
		}
		if((argc>3) && (strcmp(argv[3],"-v")==0))
			verbose=1;
	}
	else {
		if((argc>2) && (strcmp(argv[2],"-v")==0))
			verbose=1;
	}


	if( (sd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
		err_exit( argv[0], "socket creation error", 1 );
	VP("%s: tcp socket created\n",argv[0]);

	// Initialize server's address & bind it
	// Required to be in Network-Byte-Order
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = inet_addr(SERV_IP);
	svr_addr.sin_port = htons(SERV_PORT);

  if (is_server) { //--------------------------------- it's the server

	int yes=1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
		err_exit(argv[0], "setsockopt", 1);
	}
  
	if( bind(sd, (struct sockaddr *)&svr_addr,\
	      sizeof(svr_addr) ) == -1 )
		err_exit( argv[0], "svr: socket bind error", 1);
	VP("%s: bind done at IP %s port %d\n",argv[0],SERV_IP, SERV_PORT);

	if( listen(sd, 1) == -1 )
		err_exit( argv[0], "svr: socket listen error", 1);
	VP("%s: listen q set up\n",argv[0]);

	act.sa_handler=sig_child;
	sigemptyset(&act.sa_mask);
	/* On 2.6 Linux, using the SA_NOCLDWAIT flag makes it easy- zombies are prevented.
     * See man sigaction for details...
     */
	act.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_NOCLDWAIT;
	if( sigaction( SIGCHLD, &act, 0 ) == -1 )
		err_exit( argv[0], "svr: sigaction error", 1);
	VP("%s: SIGCHLD handler set up\n",argv[0]);

	while( 1 ) {
		VP("%s: pid %d blocking on accept()..\n", argv[0],getpid() );

		clilen = sizeof( cli_addr );
		if( (newsd = accept(sd, (struct sockaddr *)&cli_addr,\
		      &clilen)) == -1 )	// server blocks here..
				err_exit( argv[0], "svr: socket accept error", 1);

		VP ("\nServer %s: client connected now from IP %s \
port # %d\n", argv[0], inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

		process_client(newsd, how, argv[0]);
	} // while
  } else {  //---------------------------------------- it's the client
  	int fd_dest, trecv=0;

	// Setup the sockaddr for connecting to the server.
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = inet_addr(SERV_IP);
	svr_addr.sin_port = htons(SERV_PORT);

	// Try connecting to the server..
	if( (connect(sd, (struct sockaddr *)&svr_addr, sizeof(svr_addr))) == -1 )
		err_exit( argv[0], "cli: connection error", 1);
	
	// Xmit filename-to-send to server
	if (r_write (sd, argv[1], strlen(argv[1])) < 0)
		err_exit( argv[0], "cli: write error", 1);

	// Receive file data from server, saving into file DESTFILE
	if ((fd_dest = r_open3(DESTFILE, O_WRONLY|O_TRUNC|O_CREAT, 0644)) == -1) {
		err_exit(argv[0], "cli: dest file open error", 1);
	}
	
	while (1) {
		int nr;
		char rbuf[getpagesize()];

		nr = r_read(sd, rbuf, sizeof(rbuf));
		if (0 == nr)
			break;
		else if (nr < 0)
			err_exit( argv[0], "cli: read error", 1);

		if (r_write (fd_dest, rbuf, nr) <= 0)
			err_exit( argv[0], "cli: write error", 1);
		trecv += nr;
	}
	r_close(fd_dest);
	VP("cli: # bytes received: %16d\n", trecv);
  }

  r_close(sd);
  return 0;
} // main()

