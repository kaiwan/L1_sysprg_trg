/* 
* svr.c
* Internet domain streams socket server; concurrent / forking server.
*
* Meant to be used as a tcp socket client/server demo.
* Run the server (this code) on a Virtual Machine (VM) (like VirtualBox, 
* VmWare Player, etc)..
* As long as the VM supports NAT-based network, we'll get an IP addr ping-able from
* the host and vice-versa.
* This server now accepts CLI arguments - the (VM) IP addr and port # to assign.

* Pass this IP address as the addr parameter to the client_any app, along with
* the port # used by this server. 
* 
* Run the server:
* ./svr <IP-addr-of-VM> <port#> -v &

* On the host system, run the client (we're using a "generic" client prg, "client_any".
*  The client_any prg is in the ../beej_newer/client_any folder .)
*
* $ ./client_any <IP-addr-of-VM> <port#>
*
* and watch the fun :)

Sample Output:

On the VM:
$ ./svr 192.168.24.130 6105 -v &
[1] 10865
$ ./svr: tcp socket created
./svr: bind done at IP 192.168.24.130 port 6105
./svr: listen q set up
./svr: SIGCHLD handler set up
./svr: pid 6099 blocking on accept()..
... << blocks here (on the accept(2) ) >> ...

On the host:

$ ./client_any 192.168.24.130 6105
./client_any: issuing the getaddrinfo syscall now...
 ./client_any: loop iteration #1: got socket fd..
 ./client_any: now attempting to connect to 192.168.24.130:port # 6105...
./client_any: connected to 192.168.24.130:6105, performing 'recv' now...
./client_any: received 743 bytes, content:
'processor	: 0
vendor_id	: GenuineIntel
cpu family	: 6
model		: 42
model name	: Intel(R) Core(TM) i7-2620M CPU @ 2.70GHz
stepping	: 7
cpu MHz		: 2691.257
cache size	: 4096 KB
fdiv_bug	: no
hlt_bug		: no
f00f_bug	: no
coma_bug	: no
fpu		: yes
fpu_exception	: yes
cpuid level	: 13
wp		: yes
flags		: fpu vme de pse tsc msr pae mce cx8 apic mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss nx rdtscp lm constant_tsc up arch_perfmon pebs bts xtopology tsc_reliable nonstop_tsc aperfmperf pni pclmulqdq ssse3 cx16 sse4_1 sse4_2 popcnt aes xsave avx hypervisor lahf_lm ida arat epb xsaveopt pln pts dtherm
bogomips	: 5382.51
clflush size	: 64
cache_alignment	: 64
address sizes	: 40 bits physical, 48 bits virtual
power management:

'
$ 
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
#include <errno.h>
#include <pwd.h>

// update on your box
#include "../../../../convenient.h"

/* If you're just trying this trivially on the localhost itself, keep
 * SERV_IP as local loopback addr. Else, use the IP address of the 
 * machine it's on.
 */
#define MAXBUF		5120
#define	QLENGTH		5
#define DBG_MUST_SLEEP  0
#define	DEBUG_SLEEP	20

static int verbose = 0;

// Prevent zombies..
static void sig_child(int signum)
{
#if 0				// not reqd on 2.6+ with SA_NOCLDWAIT flag..
	int status;
	int pid;
#endif
	QP;
	return;
#if 0				// not reqd on 2.6+ with SA_NOCLDWAIT flag..
	while ((pid = wait3(&status, WNOHANG, 0)) > 0) {
		if (verbose)
			printf("parent server in SIGCHLD handler; pid=%d\n",
			       pid);
	}
#endif
}				// sig_child()

static int ErrExit(char *prg, char *err, int exitcode)
{
	char *err_str;

	err_str = (char *)malloc(300);
	if (!err_str) {
		printf("\n%s: malloc failed for err_str..\n", prg);
		printf("%s exiting with error code %d, perror shows:\n", prg,
		       exitcode);
		perror(prg);
		printf("Original error message: %s\n", err);
		exit(exitcode);
	}

	snprintf(err_str, 300, "%s: %s", prg, err);
	perror(err_str);
	free(err_str);

	exit(exitcode);
}				// ErrExit()

// Child server processes this function
static int process_client(int sd, char *prg)
{
#define LINESZ 128
	char tmpbuf[LINESZ], *reply;
	int total = 0;
	FILE *fp;

	reply = (char *)malloc(MAXBUF);
	if (!reply) {
		printf("%s: no memory for reply buffer\n", prg);
		exit(1);
	}
#if 1   /*
	 * Interesting!
	 * Commenting out the memset() that's below this comment, has
	 * valgrind complain thus:
	 * make valgrind
	 * ...
	 * valgrind --tool=memcheck --trace-children=yes \
	--track-origins=yes ./cpudtl_tstsvr_dbg 0.0.0.0 60001
 (notice how we include the '--track-origins=yes' to get to the src of the issue!)
		 * ...
==110529== Command: /usr/bin/lscpu
==110529== 
==110527== Conditional jump or move depends on uninitialised value(s)
==110527==    at 0x483EDED: strncat (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==110527==    by 0x1099F3: process_client (cpudtl_tstsvr.c:176)
==110527==    by 0x109F5D: main (cpudtl_tstsvr.c:293)
==110527==  Uninitialised value was created by a heap allocation
==110527==    at 0x483B7F3: malloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==110527==    by 0x1098BC: process_client (cpudtl_tstsvr.c:149)
==110527==    by 0x109F5D: main (cpudtl_tstsvr.c:293)
	 * 
	 * Ah, the malloc() allocs memory but its uninitialized of course...
	 * Once we init it (w/ the memset()), this err report disappears!
	 */
	memset(reply, 0, MAXBUF);
#endif

	/* !NOTE! SECURITY ALERT!
	   Doing nonsense like an arbitrary 'popen' - this can be deadly!!!
	   Esp in 'w' mode!
	 */
	fp = popen("lscpu", "r");
	if (!fp) {
		printf("%s: popen on 'lscpu' failed, trying via procfs\n", prg);
		fp = popen("cat /proc/cpuinfo", "r");
		if (!fp) {
			// TODO : write an err msg into 'reply' & send it
			free(reply);
			exit(1);
		}
	}

	while (fgets(tmpbuf, LINESZ, fp)) {
		total += strlen(tmpbuf);
		if (total > MAXBUF) {
			printf("%s: buffer limit exceeded, aborting...\n", prg);
			free(reply);
			exit(1);
		}
		strncat(reply, tmpbuf, LINESZ);
	}

	// TODO : make the write() run in a loop guaranteeing that all data is transferred
	if ((write(sd, reply, strlen(reply))) == -1) {
		free(reply);
		ErrExit(prg, "socket write error", 6);
	}

	free(reply);
	close(sd);

	return (0);
}				// process_client()

int main(int argc, char *argv[])
{
	int sd, newsd;
	socklen_t clilen;
	struct sockaddr_in svr_addr, cli_addr;
	struct sigaction act;
	int port = 49161;	//6100;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s port-num [-v]\n",
			argv[0]);
		exit(1);
	}
	port = atoi(argv[1]);

	if ((argc == 3) && (strcmp(argv[2], "-v") == 0))
		verbose = 1;

	/* Ignore SIGPIPE, so that server does not recieve it if it attempts
	   to write to a socket whose peer has closed; the write fails with EPIPE instead..
	 */
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		ErrExit(argv[0], "signal", 1);

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ErrExit(argv[0], "socket creation error", 1);
	if (verbose)
		printf("%s: tcp socket created\n", argv[0]);

	// Initialize server's address & bind it
	// Required to be in Network-Byte-Order
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = INADDR_ANY;	// INADDR_ANY == "0.0.0.0" =>any available system IPaddr
	svr_addr.sin_port = htons(port);

	if (bind(sd, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1)
		ErrExit(argv[0], "socket bind error", 2);
	/*
	 * The (in)famous 'Address already in use' issue can crop up!
	 * Explanation: http://www.softlab.ntua.gr/facilities/documentation/unix/unix-socket-faq/unix-socket-faq-4.html#ss4.2
	 * One sol- use the SO_REUSEADDR socket option
	 */
	const int enable = 1;
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		perror("Warning: setsockopt SO_REUSEADDR failed");
	if (verbose)
		printf("%s: bind done at IP %s port %d; SO_REUSEADDR setup as well\n",
			argv[0], inet_ntoa(svr_addr.sin_addr), port);

	if (listen(sd, QLENGTH) == -1)
		ErrExit(argv[0], "socket listen error", 3);
	if (verbose)
		printf("%s: listen q set up\n", argv[0]);

#define	ZOMBIES		0	// make this 1 to generate zombies! :-)
#if(ZOMBIES==0)
	act.sa_handler = sig_child;
	sigemptyset(&act.sa_mask);
	/* On 2.6 Linux, using the SA_NOCLDWAIT flag makes it easy- zombies are prevented.
	 * See man sigaction for details...
	 */
	act.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_NOCLDWAIT;
	if (sigaction(SIGCHLD, &act, 0) == -1)
		ErrExit(argv[0], "sigaction error", 4);
	if (verbose)
		printf("%s: SIGCHLD handler set up\n", argv[0]);
#endif

	while (1) {
		if (verbose)
			printf("%s: pid %d blocking on accept()..\n",
			       argv[0], getpid());
		fflush(stdout);

		clilen = sizeof(cli_addr);
		if ((newsd = accept(sd, (struct sockaddr *)&cli_addr, &clilen)) == -1)	// server blocks here..
			ErrExit(argv[0], "socket accept error", 5);
		// ... blocking ... waiting for client to connect ...

		printf("\nServer %s: client connected now from IP %s \
port # %d\n", argv[0], inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		fflush(stdout);

		// Client connected; process.. model: multiprocess concurrent (TCP) server
		switch (fork()) {
		case -1:
			perror("fork error");
			fflush(stdout);
			break;
		case 0:	// Child server
			/* for debugging with gdb/ddd etc, keep it asleep
			 * until user can have the debugger can attach to it..
			 */
			MSG("debug %s: pid is %d\n", argv[0], getpid());
#if( DBG_MUST_SLEEP == 1)
			MSG("sleeping now for %ds...\n", DEBUG_SLEEP);
			sleep(DEBUG_SLEEP);
#endif

			close(sd);
			if (verbose) {
				printf("%s: child server created; pid %d\n",
				       argv[0], getpid());
				fflush(stdout);
			}

			process_client(newsd, argv[0]);

			if (verbose) {
				printf("%s: child server %d exiting..\n",
				       argv[0], getpid());
				fflush(stdout);
			}
			exit(0);
		default:	// Parent server
			close(newsd);
		}	// switch
	}	// while
}	// main()
