/* 
*  unix_svr.c
*  Simple Unix-domain C/S demo program
*  UNIX domain connection-oriented streams socket server; concurrent server
*/
#include "unet.h"

/* clear any zombies */
static void sig_child(int signum)
{
#if 0
	int status;
	while (wait3(&status, WNOHANG, 0) > 0) ;
#else
	printf("%s(): using SA_NOCLDWAIT, so no zombie, no problem!\n", __func__);
#endif
}

int main(int argc, char *argv[])
{
	int sd, ns;
	socklen_t fromlen;
	struct sockaddr_un svr_addr, cli_addr;
	struct sigaction act;

	if (geteuid() != 0 || getuid() != 0) {
		fprintf(stderr, "%s: requires root (to create sock file under /run).\n", argv[0]);
		exit (1);
	}
	unlink(SOCKET_NAME);

	if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		perror("socket creation error"), exit(1);

	// Initialize server's address & bind it
	svr_addr.sun_family = AF_UNIX;
	strncpy(svr_addr.sun_path, SOCKET_NAME, sizeof(svr_addr.sun_path) - 1);
	if (bind(sd, (struct sockaddr *)&svr_addr, sizeof(struct sockaddr_un)) == -1)
		perror("socket bind error"), exit(1);

	// listen for connections
	if (listen(sd, 5) == -1)
		perror("socket listen error"), exit(1);

	act.sa_handler = sig_child;
	sigemptyset(&act.sa_mask);
	/* using the SA_NOCLDWAIT flag makes it easy- zombies are prevented.
	 * See man sigaction for details...
	 */
	act.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_NOCLDWAIT;
	if (sigaction(SIGCHLD, &act, 0) == -1)
		perror("signal handling error"), exit(1);

	while (1) {
		char msg[] = "hello, unix domain socket world";

		fromlen = sizeof(struct sockaddr_un);
		/* ns = new socket; the accept() returns a new socket that the svr is
		 * connected to the client upon; the original socket - sd - still exists.
		 * The parent server continues to listen for new clients on sd via accept(), whereas
		 * the child server talks to the just-connected client on the ns socket!
		 */
		ns = accept(sd, (struct sockaddr *)&cli_addr, &fromlen);	// server blocks here..
		if (ns == -1)
			perror("socket accept error"), exit(1);

		/* This is a concurrent server. So when a client connects, fork; 
		 * the child server processes the client, the parent goes back to waiting
		 * on new incoming connections...
		 */
		switch (fork()) {
		case -1:
			perror("fork error");
			fflush(stdout);
			break;

		case 0:	// Child server
			close(sd);
			if (write(ns, msg, strlen(msg)) == -1)
				perror("write error"), exit(1);
			if (shutdown(ns, SHUT_RDWR) < 0) {
				perror("shutdown"); exit(1);
			}
			exit(0);
		default:	// Parent server
			close(ns);
		}
	}

	return 0;
}
