/*
* unix_cli.c
* Simple demo client program for UNIX domain C/S;
* server program is unix_svr.c
*/
#include "unet.h"

int main(int argc, char **argv)
{
	int sd;
	ssize_t n;
	char buf[80];
	struct sockaddr_un svr_addr;

	// TODO : avoid needing root to execute
	if (geteuid() != 0 || getuid() != 0) {
		fprintf(stderr, "%s: requires root (to create sock file under /run).\n", argv[0]);
		exit (1);
	}

	// Create UNIX domain socket
	if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		perror("socket error"), exit(1);

	// connect to the server
	svr_addr.sun_family = AF_UNIX;
	strncpy(svr_addr.sun_path, SOCKET_NAME, sizeof(svr_addr.sun_path) - 1);
	// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	if (connect(sd, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) == -1)
		perror("socket connect error"), exit(1);

	/* This isn't typical; usually, the client writes a request to the server,
	 * the server processes it & replies; the client reads the reply... exits;
	 * here, we simply read from the socket, knowing that the server writes!
	 */
	if ((n = read(sd, buf, sizeof(buf))) == -1)
		perror("socket read error"), exit(1);
	buf[n] = '\0';

	printf("Client: received msg from server:\n\"%s\"\n", buf);
	//close(sd);
	if (shutdown(sd, SHUT_RDWR) < 0) {
		perror("shutdown"); exit(1);
	}
	exit(0);
}
