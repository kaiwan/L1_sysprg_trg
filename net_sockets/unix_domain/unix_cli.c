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
	struct sockaddr_un cli_addr;

	// Create UNIX domain socket
	if ((sd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1)
		perror("socket error"), exit(1);

	// connect to the server
	cli_addr.sun_family = PF_UNIX;
	strncpy(cli_addr.sun_path, SOCKET_NAME, sizeof(cli_addr.sun_path) - 1);
	if (connect(sd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) == -1)
		perror("socket connect error"), exit(1);

	// read from socket
	if ((n = read(sd, buf, sizeof(buf))) == -1)
		perror("socket read error"), exit(1);
	buf[n] = '\0';

	printf("Client: read \"%s\"\n", buf);
	close(sd);
	exit(0);
}				// main()
