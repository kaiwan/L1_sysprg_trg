/*
** client.c -- a stream socket client demo
 *
 * (c) Beej's Guide
 * http://beej.us/guide/bgnet/examples/client.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 1024	// max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes, i = 0;
	//char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	char *svrname;
	int rv;
	char s[INET6_ADDRSTRLEN];

	unsigned char msg[48]={010,0,0,0,0,0,0,0,0};    // the request to send
    //unsigned long buf[1024]; // the reply we get back
	unsigned long buf[MAXDATASIZE];

	if (argc != 3) {
		fprintf(stderr, "usage: %s hostname port#\n", argv[0]);
		exit(1);
	}

    /**
	 * 1. Estabilish a connection to an available NTP server
	 */
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_ALL	// IMP: return both IPv6 and IPv4-mapped IPv6 addresses in the list
	    | AI_CANONNAME;     // return name

	/*
	   From the man page on getaddrinfo(3):
	   int getaddrinfo(const char *node, const char *service,
	   const struct addrinfo *hints,
	   struct addrinfo **res);
	   ...
	   Given node and service, which identify an Internet host and a service, 
	   getaddrinfo() returns one or more addrinfo structures, each of which
	   contains an Internet address that can be specified in a call to bind(2) or 
	   connect(2). The getaddrinfo() function combines the functionality provided 
	   by the getservbyname(3) and getservbyport(3) functions into a single interface, 
	   but unlike the latter functions, getaddrinfo() is reentrant and allows programs 
	   to eliminate IPv4-versus-IPv6 dependencies.
	   ...
	   The hints argument points to an addrinfo structure that specifies criteria for 
	   selecting the socket address structures returned in the list pointed to by res.  
	   If hints is not NULL it points to an addrinfo structure whose ai_family, 
	   ai_socktype, and ai_protocol specify criteria that limit the set of socket 
	   addresses returned by getaddrinfo(),
	   ...
	 */
	printf("%s: issuing the getaddrinfo lib now...\n", argv[0]);
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	/*
	   ...
	   The getaddrinfo() function allocates and initializes a linked list of addrinfo 
	   structures, one for each network address that  matches  node and  service, 
	   subject to any restrictions imposed by hints, and returns a pointer to the start 
	   of the list in res.  The items in the linked list are linked by the ai_next field.
	   ...      
	 */

	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				     p->ai_protocol)) == -1) {
			perror("client_any: socket");
			continue;
		}
		printf(" %s: loop iteration #%d: got socket fd..\n", argv[0],
		       ++i);

		printf
		    (" %s: now attempting to connect to %s:%d...canonical name: %s\n",
		     argv[0], argv[1], atoi(argv[2]), servinfo->ai_canonname);
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client_any: connect failed");
			continue;
		}
		//strncpy(svrname, (char *)servinfo->ai_canonname, 256);
		svrname = servinfo->ai_canonname;
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client_any: failed to connect\n");
		return 2;
	}
	// Convert the IP to a string and print it:
	//  p = presentation (dotted-decimal str), n = network (numeric)
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
		  sizeof s);
	printf("%s: connected to server %s [IP %s:%d] [socktype %s proto # %d]\n",
	       argv[0], svrname, s, atoi(argv[2]), 
		   (p->ai_socktype==SOCK_STREAM?"tcp":"udp"), p->ai_protocol);


    /**
	 * 2. Send request to NTP server
	 */
	printf("%s: sending request to NTP server now...\n", argv[0]);
	//if ((sendto(sockfd, msg, sizeof(msg), 0, p->ai_addr, sizeof(p->ai_addr))) == -1) {
	if ((sendto(sockfd, msg, sizeof(msg), 0, p->ai_addr, 16)) == -1) {
		perror("ntp_cli: sendto failed");
		freeaddrinfo(servinfo);
		close(sockfd);
		return 3;
	}

    /**
	 * 3. Receive reply from NTP server
	 */
	struct sockaddr src_addr;
	socklen_t saddr_len = sizeof(src_addr);
#if 1
	printf("%s: awaiting reply from NTP server now...\n", argv[0]);
	// recv(2) will work for both tcp stream and datagram sockets
	//if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
	if ((numbytes = recvfrom(sockfd, buf, 48, 0, &src_addr, &saddr_len)) == -1) {
		perror("recv");
		exit(1);
	}
	if (0 == numbytes) {
		printf("%s: peer closed the connection! (recv ret 0)\n",
		       argv[0]);
		close(sockfd);
		return 0;
	}
	buf[numbytes] = '\0';
	printf("%s: received %d bytes, content:\n'%.*s'\n", argv[0], numbytes,
	       numbytes, buf);
#endif

	freeaddrinfo(servinfo);	// all done with this structure
	close(sockfd);
	return 0;
}
